#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include "SharedQueue.h"

class Consumer
{
private:
    typedef std::vector<std::string> csv_row;

public:
    Consumer();
    ~Consumer();
    void run();

    void set_mutex(std::mutex *m);

    void push_job(csv_row row);

    void generate_report();

private:
    csv_row pull_job();
    std::mutex *mut;
    SharedQueue job_queue;
    std::string protocol_type;
};
