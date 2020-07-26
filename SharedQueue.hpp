#pragma once
#include <mutex>
#include <queue>
#include <string>

class SharedQueue
{
public:
    SharedQueue();
    ~SharedQueue();

    /**
     * Assign mutex from consumer.
     * 
     * @param m mutex pointer.
     */
    void set_mutex(std::mutex *m);

    /**
     * Pop from queue safely.
     * 
     * @return csv_row.
     */
    std::vector<std::string> pop_front();

    /**
     * push on to queue safely.
     * 
     * @param row csv_row to push.
     */
    void push_back(std::vector<std::string> row);

    /**
     * Return true if the queue is empty.
     * 
     * @return true if empty.
     */
    bool empty();

private:
    /**
     * Interesting method which I used for experimenting with mutexes
     * 
     * @param operation flag for what task I wished to be compeleted.
     * @param row If applicable to row to be pushed.
     * 
     * @return if applicable, poped csv_row.
     */
    std::vector<std::string> change_queue(int operation, std::vector<std::string> row);

    std::mutex *mut;                               // Mutex pointer
    std::queue<std::vector<std::string>> my_queue; // Standard c++ queue
};