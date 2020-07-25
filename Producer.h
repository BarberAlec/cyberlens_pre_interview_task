#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <mutex>
#include <thread>
#include "Consumer.h"

class Producer
{
public:
    Producer();
    ~Producer();

    /**
     * Begins interation with user.
     */
    void start();

    /***
     * Specify path to csv data.
     * 
     * @param file_dir path to csv file.
     */
    void set_data_directory(std::string file_dir);

private:
    typedef std::vector<std::string> csv_row;

    void print_menu();

    void run();

    void terminate_consumers();

    /**
     * Parse next line of CSV file.
     *
     * @return string vector of entries in row.
     */
    csv_row read_csv_entry();

    /**
     * Pushes given row to appropiate job queue.
     * 
     * @param row Vector of Strings defining entry.
     * @return true if success, false if no good queue.
     */
    bool push_to_queue(csv_row row);

    /**
     * Instantiates three new threads for each consumer
     * 
     * @return true if success.
     */
    bool begin_consumer_threads();

    struct csv_idx
    {
        const int idx = 0;
        const int duration = 1;
        const int protocol_type = 2;
        const int service = 3;
        const int flag = 4;
        const int src_bytes = 5;
        const int dst_bytes = 6;
        const int land = 7;
        const int wrong_fragment = 8;
        const int urgent = 9;
        const int hot = 10;
    };

    // Find row element by colomn helper.
    csv_idx file_idx;

    // CSV file directory and associated fstream object.
    std::string file_path;
    std::fstream csv_file;

    // Consumers
    Consumer TCP_consumer, ICMP_consumer, UDP_consumer;

    // Consumer thread
    std::thread *TCP_thrd, *ICMP_thrd, *UDP_thrd;

    // Mutex for guarding shared queues
    std::mutex TCP_mutex, IMCP_mutex, UDP_mutex;

    // Flag for threads
    bool threads_active;
};