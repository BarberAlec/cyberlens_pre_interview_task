#include "Producer.h"

Producer::Producer()
{
    threads_active = false;

    // Assign mutex to each consumer
    TCP_consumer.set_mutex(&TCP_mutex);
    ICMP_consumer.set_mutex(&IMCP_mutex);
    UDP_consumer.set_mutex(&UDP_mutex);
}

Producer::~Producer()
{
    csv_file.close();

    if (threads_active)
    {
        terminate_consumers();
        // TCP_thrd->join();
        // ICMP_thrd->join();
        // UDP_thrd->join();
        delete TCP_thrd;
        delete ICMP_thrd;
        delete UDP_thrd;
    }
}

void Producer::set_data_directory(std::string file_dir)
{
    file_path = file_dir;
}

void Producer::run()
{
    // Launch consumer threads
    threads_active = begin_consumer_threads();

    // Open file and throw out header
    csv_file.open(file_path, std::ios::in);
    read_csv_entry();
    //std::cout << "thing" << std::endl;

    // Iterate through each entry in csv
    while (!csv_file.eof())
    {
        // Read entry
        csv_row curr_row = read_csv_entry();
        if (curr_row.empty())
        {
            return;
        }
        //std::cout << curr_row[file_idx.idx] << " , " << curr_row[file_idx.hot] << std::endl;

        // Push to queue
        if (!push_to_queue(curr_row))
        {
            std::cout << "Error adding row: " << curr_row[file_idx.idx] << std::endl;
        }
    }

    // Finished packet stream, send terminate message to threads
    terminate_consumers();
}

void Producer::terminate_consumers()
{
    csv_row terminate;
    terminate.push_back("END");
    for (int i = 1; i < 12; i++)
    {
        terminate.push_back("");
    }

    terminate[file_idx.protocol_type] = "tcp";
    push_to_queue(terminate);

    terminate[file_idx.protocol_type] = "icmp";
    push_to_queue(terminate);

    terminate[file_idx.protocol_type] = "udp";
    push_to_queue(terminate);

    TCP_thrd->join();
    ICMP_thrd->join();
    UDP_thrd->join();
    threads_active = false;
}

bool Producer::begin_consumer_threads()
{
    TCP_thrd = new std::thread(&Consumer::run, TCP_consumer);
    ICMP_thrd = new std::thread(&Consumer::run, ICMP_consumer);
    UDP_thrd = new std::thread(&Consumer::run, UDP_consumer);

    return true;
}

void Producer::print_menu()
{
    std::cout << "......................MENU......................" << std::endl;
    std::cout << "To begin processing:                       start" << std::endl;
    std::cout << "To generate reports:                      report" << std::endl;
    std::cout << "To stop and exit:                           stop" << std::endl
              << std::endl;
}

void Producer::start()
{
    std::string user_input;

    std::cout << "Welcome. This packet parsing simulation was built by Alec Barber for cyberLens." << std::endl;
    std::cout << "Documentation can be found at ___" << std::endl
              << std::endl;
    std::cout << "If you have any further questions with regards to this softwares exection, \nplease contact me at barberal@tcd.ie"
              << std::endl
              << std::endl
              << std::endl;

    // Present menu to user
    print_menu();

    // TODO: make robust to case.
    std::cin >> user_input;
    while (user_input.compare("stop") != 0)
    {
        if (user_input.compare("start") == 0)
        {
            // User selected start option
            std::cout << "Begining processing of packet stream." << std::endl
                      << std::endl;
            run();
        }
        else if (user_input.compare("report") == 0)
        {
            // User selected report option
            std::cout << "Generating and printing reports." << std::endl
                      << std::endl;
            TCP_consumer.generate_report();
        }
        else if (user_input.empty())
        {
            // User entered empty command, do not reprint menu.
            std::cin >> user_input;
            continue;
        }
        else
        {
            // User entered unrecognised command, reprint menu.
            std::cout << "Unrecognised command: " << user_input << std::endl;
            std::cout << "Please enter another command." << std::endl;
            print_menu();
        }

        // Processed command, now accept next.
        std::cin >> user_input;
    }

    std::cout << "Exiting with grace and reverence." << std::endl;
}

Producer::csv_row Producer::read_csv_entry()
{
    csv_row row;
    std::string line, word;

    // Get next line, if end of file, then return empty row
    std::getline(csv_file, line);
    if (line.empty())
    {
        return row;
    }
    std::stringstream ss(line);
    // Seperate row into a vector of entries
    while (std::getline(ss, word, ','))
    {
        row.push_back(word);
    }

    return row;
}

bool Producer::push_to_queue(csv_row row)
{
    std::string protocol_type = row[(file_idx.protocol_type)];

    if (protocol_type.compare("tcp") == 0)
    {
        TCP_consumer.push_job(row);
    }
    else if (protocol_type.compare("icmp") == 0)
    {
        ICMP_consumer.push_job(row);
    }
    else if (protocol_type.compare("udp") == 0)
    {
        UDP_consumer.push_job(row);
    }
    else
    {
        return false;
    }
    return true;
}