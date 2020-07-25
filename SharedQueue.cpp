#include "SharedQueue.h"

SharedQueue::SharedQueue()
{
}

SharedQueue::~SharedQueue()
{
}

void SharedQueue::set_mutex(std::mutex *m)
{
    mut = m;
}

std::vector<std::string> SharedQueue::pop_front()
{
    std::lock_guard<std::mutex> lock(*mut);

    std::vector<std::string> ans = my_queue.front();
    my_queue.pop();

    return ans;
}

void SharedQueue::push_back(std::vector<std::string> row)
{
    std::lock_guard<std::mutex> lock(*mut);
    my_queue.push(row);
}