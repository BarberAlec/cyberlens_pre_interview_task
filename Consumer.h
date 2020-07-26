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
    // Enumerated csv_row, faster to make comparisons with ints (relative to string comparisons)
    typedef std::vector<int> enum_csv_row;

public:
    Consumer(std::string protocol);
    Consumer();
    ~Consumer();
    void run();

    void set_mutex(std::mutex *m);

    void push_job(csv_row row);

    void push_cmd_job(csv_row row);

    void generate_report();

    void set_protocol_type(std::string protocol);

private:
    // Encapsulation of a unique packet and associated meta data.
    typedef struct
    {
        csv_row entry;
        enum_csv_row enum_entry;
        std::vector<int> indices;
        int count;
    } unique_packet;

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

    csv_idx file_idx;                           // Helper for indexing csv_row
    std::mutex *mut;                            // Shared mutex object
    SharedQueue job_queue;                      // Queue for data packets
    SharedQueue producer_cmds;                  // Queue for Producer commands
    std::string protocol_type;                  // tcd, icmp or udp
    std::vector<unique_packet> unq_pkt_list;    // Vector of unique data packets observed

    csv_row pull_job();

    bool do_job_queue();

    void do_job(csv_row entry);

    bool check_for_cmd();
    
    /**
     * If packet already exists, adds increment to count and index info,
     * else make a new unique packet entry.
     * 
     * @param pkt new packet to add to database
     * @return true if new entry made, false if packet already exists
     */
    bool add_packet(unique_packet new_pkt);

    bool compare_packets(unique_packet pkt1, unique_packet pkt2);

    enum_csv_row make_enum_entry(csv_row entry);
    
};
