#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include "SharedQueue.h"

class Consumer
{
public:
    Consumer();
    Consumer(std::string protocol);

    ~Consumer();

    typedef std::vector<std::string> csv_row;

    /**
     * Begin main operation of Consumer thread. The consumer will begin checking for data on
     * the shared queues. Operation will cease when an END command is sent by the producer in 
     * the producer_cmds queue. If no jobs left on queue, then thread will sleep. 
     * 
     * (TODO:implement exponential backoff for checking queue for jobs to save resources).
     */
    void begin_thread();

    /**
     * Public method assigning a job (ie a csv row / packet) to the consumer by the Producer.
     * 
     * @param row csv row to be assigned to the consumer.
     */
    void push_job(csv_row row);

    /**
     * Public method assinging a meta command to the consumer from the Producer. E.g. sending a 
     * REPORT or END request. Using a seperate queue allows for better reaction rates by the 
     * consumer.
     * 
     * @param row csv row with associated command.
     */
    void push_cmd_job(csv_row row);

    /**
     * Set consumer as either tcp, icmp or udp.
     * 
     * @param protocol
     */
    void set_protocol_type(std::string protocol);

    /**
     * Mutex is provided by the producer and is assigned to each consumer. Consumer in turn 
     * passes the mutext to its SharedQueues.
     * 
     * @param m pointer to a mutex.
     */
    void set_mutex(std::mutex *m);

private:
    // Enumerated csv_row, faster to make comparisons with natural numbers
    // (relative to string comparisons).
    typedef std::vector<int> enum_csv_row;

    // Container for each unique packet and meta data.
    typedef struct
    {
        csv_row entry;            // Original csv row.
        enum_csv_row enum_entry;  // Enumerated csv row.
        std::vector<int> indices; // Collection of indices where packets are copies.
        int count;                // Number of packets with same data.
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

    csv_idx file_idx;                        // Helper for indexing csv_row.
    std::mutex *mut;                         // Shared mutex object.
    SharedQueue job_queue;                   // Queue for data packets.
    SharedQueue producer_cmds;               // Queue for Producer commands.
    std::string protocol_type;               // tcd, icmp or udp.
    std::vector<unique_packet> unq_pkt_list; // Vector of unique data packets observed.

    /**
     * Generate and print consumer report to file.
     */
    void generate_report();
    
    /**
     * Pull next task on job queue.
     * 
     * @return csv row describing job.
     */
    csv_row pull_job();

    /**
     * Check for new commands from producer and jobs on the queue. Continues to do jobs till a 
     * END command recieved from the producer or there are no jobs on the queue left.
     * 
     * @return false if no END command received, else true.
     */
    bool do_job_queue();

    /**
     * Handles creation of new unique_packet object and passes on to add_packet.
     * 
     * @param entry csv_row to be processed.
     */
    void do_job(csv_row entry);

    /**
     * Check for command on command queue.
     * 
     * @return true if END command recieved.
     */
    bool check_for_cmd();

    /**
     * If packet already exists, adds increment to count and index info,
     * else make a new unique packet entry.
     * 
     * @param pkt new packet to add to database
     * @return true if new entry made, false if packet already exists
     */
    bool add_packet(unique_packet new_pkt);

    /**
     * Utility fuction testing equality between two packets.
     * 
     * @param pkt1 First packet to compare.
     * @param pkt2 Second packet to compare.
     * 
     * @return true if data fields equal, else false.
     */
    bool compare_packets(unique_packet pkt1, unique_packet pkt2);

    /**
     * Converts a normal csv_row object (vector of strings) to a 
     * enumerated vector of ints. (i.e. changing categorical data to ints).
     * 
     * @param entry csv row to translate.
     * @return enumerated csv row.
     */
    enum_csv_row make_enum_entry(csv_row entry);
};
