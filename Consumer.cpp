#include <fstream>
#include "Consumer.h"
#include <ctime>
#include <chrono>

using namespace std::chrono_literals;

Consumer::Consumer(std::string protocol)
{
    protocol_type = protocol;
}

Consumer::Consumer()
{
    protocol_type = "tcp";
}

Consumer::~Consumer()
{
}

void Consumer::run()
{
    std::cout << "Starting thread" << std::endl;
    bool datastream_finished = false;

    // Check for job
    datastream_finished = do_job_queue();
    while (!datastream_finished)
    {
        // Queue empty, but no terminate message recieved, begin backoff
        // TODO: implement exponential backoff
        std::this_thread::sleep_for(1s);

        // Search for new job again.
        datastream_finished = do_job_queue();
    }
}

void Consumer::set_protocol_type(std::string protocol)
{
    protocol_type = protocol;
}

bool Consumer::check_for_cmd()
{
    csv_row curr_entry;
    if (!producer_cmds.empty())
    {
        curr_entry = producer_cmds.pop_front();

        // Check for terminate message
        if (curr_entry[0].compare("END") == 0)
        {
            // Last packet recieved
            std::cout << "Received termination signal" << std::endl;
            return true;
        }

        // Check for a report generation request
        if (curr_entry[0].compare("REPORT") == 0)
        {
            std::cout << "Recieved report signal" << std::endl;
            generate_report();
            return false;
        }
    }

    // didnt understand cmd, ignore
    return false;
}

bool Consumer::do_job_queue()
{
    csv_row curr_entry;
    bool terminate;

    terminate = check_for_cmd();
    if (terminate) 
        return true;
        
    while (!job_queue.empty())
    {
        if (terminate)
        {
            return true;
        }

        // Check for new packet jobs
        curr_entry = job_queue.pop_front();

        // Do some processing
        do_job(curr_entry);

        // Check for new Producer commands
        terminate = check_for_cmd();
    }

    // Queue empty, but no termaination, return to parent function and wait.
    return false;
}

void Consumer::do_job(csv_row entry)
{
    unique_packet new_pkt;

    //Create temp entry without index
    csv_row temp = entry;
    temp.erase(temp.begin());

    new_pkt.count = 1;
    new_pkt.entry = temp;
    new_pkt.enum_entry = make_enum_entry(entry);
    new_pkt.indices.push_back(std::stoi(entry[0]));

    add_packet(new_pkt);
}

bool Consumer::compare_packets(unique_packet pkt1, unique_packet pkt2)
{
    if (pkt1.enum_entry[file_idx.dst_bytes] != pkt1.enum_entry[file_idx.dst_bytes])
        return false;
    if (pkt1.enum_entry[file_idx.src_bytes] != pkt1.enum_entry[file_idx.src_bytes])
        return false;
    if (pkt1.enum_entry[file_idx.protocol_type] != pkt1.enum_entry[file_idx.protocol_type])
        return false;
    if (pkt1.enum_entry[file_idx.service] != pkt1.enum_entry[file_idx.service])
        return false;
    if (pkt1.enum_entry[file_idx.duration] != pkt1.enum_entry[file_idx.duration])
        return false;
    if (pkt1.enum_entry[file_idx.flag] != pkt1.enum_entry[file_idx.flag])
        return false;
    if (pkt1.enum_entry[file_idx.hot] != pkt1.enum_entry[file_idx.hot])
        return false;
    if (pkt1.enum_entry[file_idx.land] != pkt1.enum_entry[file_idx.land])
        return false;
    if (pkt1.enum_entry[file_idx.urgent] != pkt1.enum_entry[file_idx.urgent])
        return false;
    if (pkt1.enum_entry[file_idx.wrong_fragment] != pkt1.enum_entry[file_idx.wrong_fragment])
        return false;

    return true;
}

bool Consumer::add_packet(unique_packet new_pkt)
{
    // TODO: make this much better...
    // not great performance when considering large numbers of unqiue packets
    // Some sort of hash table could suffice.

    // I have made this more efficent as I search from the end of the vector as one
    // would expect to find a duplicate more recently.

    for (std::vector<unique_packet>::reverse_iterator pkt = unq_pkt_list.rbegin();
         pkt != unq_pkt_list.rend(); ++pkt)
    {
        if (compare_packets(new_pkt, *pkt))
        {
            pkt->count++;
            pkt->indices.push_back(new_pkt.indices[0]);
            return false;
        }
    }

    // no match found, push to end
    unq_pkt_list.push_back(new_pkt);
    return true;
}

void Consumer::set_mutex(std::mutex *m)
{
    // can use same mutex
    mut = m;
    job_queue.set_mutex(mut);
    producer_cmds.set_mutex(mut);
}

void Consumer::push_job(csv_row row)
{
    job_queue.push_back(row);
}

void Consumer::push_cmd_job(csv_row row)
{    
    producer_cmds.push_back(row);
}

void Consumer::generate_report()
{
    std::cout << "Consumer " << protocol_type << " generating report." << std::endl;
    std::string file_path = "reports/" + protocol_type + "_report_" + std::to_string(time(NULL)) + ".txt";

    std::fstream report;
    report.open(file_path, std::ios::out);

    report << "test output" << std::endl;

    report.close();
}

Consumer::enum_csv_row Consumer::make_enum_entry(csv_row entry)
{
    enum_csv_row new_enum_row(11);

    // Ignore idx
    new_enum_row[file_idx.idx] = 0;

    // Duration
    new_enum_row[file_idx.duration] = std::stoi(entry[file_idx.duration]);

    // Protocol
    if (entry[file_idx.protocol_type].compare("tcp") == 0)
    {
        new_enum_row[file_idx.protocol_type] = 0;
    }
    else if (entry[file_idx.protocol_type].compare("icmp") == 0)
    {
        new_enum_row[file_idx.protocol_type] = 1;
    }
    else if (entry[file_idx.protocol_type].compare("udp") == 0)
    {
        new_enum_row[file_idx.protocol_type] = 2;
    }
    else
    {
        std::cout << "ERROR Consumer::enum_csv_row Consumer::make_enum_entry, unrecognised protocol: " << entry[file_idx.protocol_type] << std::endl;
        new_enum_row[file_idx.protocol_type] = 3;
    }

    // src_bytes
    new_enum_row[file_idx.src_bytes] = std::stoi(entry[file_idx.src_bytes]);

    // dst_bytes
    new_enum_row[file_idx.dst_bytes] = std::stoi(entry[file_idx.dst_bytes]);

    // land
    new_enum_row[file_idx.land] = std::stoi(entry[file_idx.land]);

    // wrong_fragment
    new_enum_row[file_idx.wrong_fragment] = std::stoi(entry[file_idx.wrong_fragment]);

    // urgent
    new_enum_row[file_idx.urgent] = std::stoi(entry[file_idx.urgent]);

    // hot
    new_enum_row[file_idx.hot] = std::stoi(entry[file_idx.hot]);

    // Service type (note I have the most likely services at the top)
    if (entry[file_idx.service].compare("ecr_i") == 0)
    {
        new_enum_row[file_idx.service] = 9;
    }
    else if (entry[file_idx.service].compare("private") == 0)
    {
        new_enum_row[file_idx.service] = 11;
    }
    else if (entry[file_idx.service].compare("http") == 0)
    {
        new_enum_row[file_idx.service] = 0;
    }
    else if (entry[file_idx.service].compare("smpt") == 0)
    {
        new_enum_row[file_idx.service] = 1;
    }
    else if (entry[file_idx.service].compare("other") == 0)
    {
        new_enum_row[file_idx.service] = 10;
    }
    else if (entry[file_idx.service].compare("domain_u") == 0)
    {
        new_enum_row[file_idx.service] = 3;
    }
    else if (entry[file_idx.service].compare("ftp_data") == 0)
    {
        new_enum_row[file_idx.service] = 13;
    }
    else if (entry[file_idx.service].compare("eco_i") == 0)
    {
        new_enum_row[file_idx.service] = 7;
    }
    else if (entry[file_idx.service].compare("finger") == 0)
    {
        new_enum_row[file_idx.service] = 2;
    }
    else if (entry[file_idx.service].compare("ftp") == 0)
    {
        new_enum_row[file_idx.service] = 6;
    }
    else if (entry[file_idx.service].compare("auth") == 0)
    {
        new_enum_row[file_idx.service] = 4;
    }
    else if (entry[file_idx.service].compare("telnet") == 0)
    {
        new_enum_row[file_idx.service] = 5;
    }
    else if (entry[file_idx.service].compare("ntp_u") == 0)
    {
        new_enum_row[file_idx.service] = 8;
    }
    else if (entry[file_idx.service].compare("pop_3") == 0)
    {
        new_enum_row[file_idx.service] = 12;
    }
    else if (entry[file_idx.service].compare("rje") == 0)
    {
        new_enum_row[file_idx.service] = 14;
    }
    else if (entry[file_idx.service].compare("time") == 0)
    {
        new_enum_row[file_idx.service] = 15;
    }
    else if (entry[file_idx.service].compare("mtp") == 0)
    {
        new_enum_row[file_idx.service] = 16;
    }
    else if (entry[file_idx.service].compare("link") == 0)
    {
        new_enum_row[file_idx.service] = 17;
    }
    else if (entry[file_idx.service].compare("remote_job") == 0)
    {
        new_enum_row[file_idx.service] = 18;
    }
    else if (entry[file_idx.service].compare("gopher") == 0)
    {
        new_enum_row[file_idx.service] = 19;
    }
    else if (entry[file_idx.service].compare("ssh") == 0)
    {
        new_enum_row[file_idx.service] = 20;
    }
    else if (entry[file_idx.service].compare("name") == 0)
    {
        new_enum_row[file_idx.service] = 21;
    }
    else if (entry[file_idx.service].compare("whois") == 0)
    {
        new_enum_row[file_idx.service] = 22;
    }
    else if (entry[file_idx.service].compare("domain") == 0)
    {
        new_enum_row[file_idx.service] = 23;
    }
    else if (entry[file_idx.service].compare("login") == 0)
    {
        new_enum_row[file_idx.service] = 24;
    }
    else if (entry[file_idx.service].compare("imap4") == 0)
    {
        new_enum_row[file_idx.service] = 25;
    }
    else if (entry[file_idx.service].compare("daytime") == 0)
    {
        new_enum_row[file_idx.service] = 26;
    }
    else if (entry[file_idx.service].compare("ctf") == 0)
    {
        new_enum_row[file_idx.service] = 27;
    }
    else if (entry[file_idx.service].compare("nntp") == 0)
    {
        new_enum_row[file_idx.service] = 28;
    }
    else if (entry[file_idx.service].compare("shell") == 0)
    {
        new_enum_row[file_idx.service] = 29;
    }
    else if (entry[file_idx.service].compare("IRC") == 0)
    {
        new_enum_row[file_idx.service] = 30;
    }
    else if (entry[file_idx.service].compare("nnsp") == 0)
    {
        new_enum_row[file_idx.service] = 31;
    }
    else if (entry[file_idx.service].compare("http_443") == 0)
    {
        new_enum_row[file_idx.service] = 32;
    }
    else if (entry[file_idx.service].compare("exec") == 0)
    {
        new_enum_row[file_idx.service] = 33;
    }
    else if (entry[file_idx.service].compare("printer") == 0)
    {
        new_enum_row[file_idx.service] = 34;
    }
    else if (entry[file_idx.service].compare("efs") == 0)
    {
        new_enum_row[file_idx.service] = 35;
    }
    else if (entry[file_idx.service].compare("courier") == 0)
    {
        new_enum_row[file_idx.service] = 36;
    }
    else if (entry[file_idx.service].compare("uucp") == 0)
    {
        new_enum_row[file_idx.service] = 37;
    }
    else if (entry[file_idx.service].compare("klogin") == 0)
    {
        new_enum_row[file_idx.service] = 38;
    }
    else if (entry[file_idx.service].compare("kshell") == 0)
    {
        new_enum_row[file_idx.service] = 39;
    }
    else if (entry[file_idx.service].compare("echo") == 0)
    {
        new_enum_row[file_idx.service] = 40;
    }
    else if (entry[file_idx.service].compare("discard") == 0)
    {
        new_enum_row[file_idx.service] = 41;
    }
    else if (entry[file_idx.service].compare("systat") == 0)
    {
        new_enum_row[file_idx.service] = 42;
    }
    else if (entry[file_idx.service].compare("supdup") == 0)
    {
        new_enum_row[file_idx.service] = 43;
    }
    else if (entry[file_idx.service].compare("iso_tsap") == 0)
    {
        new_enum_row[file_idx.service] = 44;
    }
    else if (entry[file_idx.service].compare("hostnames") == 0)
    {
        new_enum_row[file_idx.service] = 45;
    }
    else if (entry[file_idx.service].compare("csnet_ns") == 0)
    {
        new_enum_row[file_idx.service] = 46;
    }
    else if (entry[file_idx.service].compare("pop_2") == 0)
    {
        new_enum_row[file_idx.service] = 47;
    }
    else if (entry[file_idx.service].compare("sunrpc") == 0)
    {
        new_enum_row[file_idx.service] = 48;
    }
    else if (entry[file_idx.service].compare("uucp_path") == 0)
    {
        new_enum_row[file_idx.service] = 49;
    }
    else if (entry[file_idx.service].compare("netbios_ns") == 0)
    {
        new_enum_row[file_idx.service] = 50;
    }
    else if (entry[file_idx.service].compare("netbios_ssn") == 0)
    {
        new_enum_row[file_idx.service] = 51;
    }
    else if (entry[file_idx.service].compare("netbios_dgm") == 0)
    {
        new_enum_row[file_idx.service] = 52;
    }
    else if (entry[file_idx.service].compare("sql_net") == 0)
    {
        new_enum_row[file_idx.service] = 53;
    }
    else if (entry[file_idx.service].compare("vmnet") == 0)
    {
        new_enum_row[file_idx.service] = 54;
    }
    else if (entry[file_idx.service].compare("bgp") == 0)
    {
        new_enum_row[file_idx.service] = 55;
    }
    else if (entry[file_idx.service].compare("Z39_50") == 0)
    {
        new_enum_row[file_idx.service] = 56;
    }
    else if (entry[file_idx.service].compare("ldap") == 0)
    {
        new_enum_row[file_idx.service] = 57;
    }
    else if (entry[file_idx.service].compare("netstat") == 0)
    {
        new_enum_row[file_idx.service] = 58;
    }
    else if (entry[file_idx.service].compare("urh_i") == 0)
    {
        new_enum_row[file_idx.service] = 59;
    }
    else if (entry[file_idx.service].compare("X11") == 0)
    {
        new_enum_row[file_idx.service] = 60;
    }
    else if (entry[file_idx.service].compare("urp_i") == 0)
    {
        new_enum_row[file_idx.service] = 61;
    }
    else if (entry[file_idx.service].compare("pm_dump") == 0)
    {
        new_enum_row[file_idx.service] = 62;
    }
    else if (entry[file_idx.service].compare("tftp_u") == 0)
    {
        new_enum_row[file_idx.service] = 63;
    }
    else if (entry[file_idx.service].compare("tftp_u") == 0)
    {
        new_enum_row[file_idx.service] = 64;
    }
    else if (entry[file_idx.service].compare("tim_i") == 0)
    {
        new_enum_row[file_idx.service] = 65;
    }
    else if (entry[file_idx.service].compare("red_i") == 0)
    {
        new_enum_row[file_idx.service] = 66;
    }
    else
    {
        std::cout << "ERROR Consumer::enum_csv_row Consumer::make_enum_entry, unrecognised service: " << entry[file_idx.service] << std::endl;
        new_enum_row[file_idx.service] = 67;
    }

    // Flag
    if (entry[file_idx.flag].compare("SF") == 0)
    {
        new_enum_row[file_idx.flag] = 0;
    }
    else if (entry[file_idx.flag].compare("S0") == 0)
    {
        new_enum_row[file_idx.flag] = 1;
    }
    else if (entry[file_idx.flag].compare("REJ") == 0)
    {
        new_enum_row[file_idx.flag] = 2;
    }
    else if (entry[file_idx.flag].compare("RSTR") == 0)
    {
        new_enum_row[file_idx.flag] = 3;
    }
    else if (entry[file_idx.flag].compare("RSTO") == 0)
    {
        new_enum_row[file_idx.flag] = 4;
    }
    else if (entry[file_idx.flag].compare("SH") == 0)
    {
        new_enum_row[file_idx.flag] = 5;
    }
    else if (entry[file_idx.flag].compare("S1") == 0)
    {
        new_enum_row[file_idx.flag] = 6;
    }
    else if (entry[file_idx.flag].compare("S2") == 0)
    {
        new_enum_row[file_idx.flag] = 7;
    }
    else if (entry[file_idx.flag].compare("RSTOS0") == 0)
    {
        new_enum_row[file_idx.flag] = 8;
    }
    else if (entry[file_idx.flag].compare("S3") == 0)
    {
        new_enum_row[file_idx.flag] = 9;
    }
    else if (entry[file_idx.flag].compare("OTH") == 0)
    {
        new_enum_row[file_idx.flag] = 10;
    }
    else
    {
        std::cout << "ERROR: could not find flag: " << new_enum_row[file_idx.flag] << std::endl;
        new_enum_row[file_idx.flag] = 11;
    }

    return new_enum_row;
}
