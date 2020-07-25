#include <fstream>
#include "Consumer.h"
#include <ctime>

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
}

void Consumer::set_mutex(std::mutex *m)
{
    mut = m;
    job_queue.set_mutex(mut);
}

void Consumer::push_job(csv_row row)
{
    job_queue.push_back(row);
}

void Consumer::generate_report()
{
    //time_t curr_time = time(NULL);
    std::string file_path = "reports/" + protocol_type + "_report_" + std::to_string(time(NULL)) + ".txt";

    std::fstream report;
    report.open(file_path, std::ios::out);

    report << "test output" << std::endl;

    report.close();
}