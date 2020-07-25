#pragma once
#include <mutex>
#include <queue>
#include <string>

class SharedQueue
{
public:
    SharedQueue();
    ~SharedQueue();

    void set_mutex(std::mutex *m);

    std::vector<std::string> pop_front();

    void push_back(std::vector<std::string> row);

private:
    std::mutex *mut;
    std::queue<std::vector<std::string>> my_queue;
};