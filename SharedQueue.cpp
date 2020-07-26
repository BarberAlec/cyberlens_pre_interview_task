#include "SharedQueue.hpp"

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
    //std::lock_guard<std::mutex> lock(*mut);
    //std::lock_guard<std::mutex> lock(g_mut);

    // std::vector<std::string> ans = my_queue.front();
    // my_queue.pop();

    // return ans;
    std::vector<std::string> dummy;
    return change_queue(0, dummy);
}

void SharedQueue::push_back(std::vector<std::string> row)
{
    //std::lock_guard<std::mutex> lock(*mut);
    //std::lock_guard<std::mutex> lock(g_mut);
    // my_queue.push(row);
    change_queue(1, row);
}

bool SharedQueue::empty()
{
    // Reading memory should be safe so no lock
    //std::lock_guard<std::mutex> lock(*mut);
    //std::lock_guard<std::mutex> lock(g_mut);
    return my_queue.empty();
}

std::vector<std::string> SharedQueue::change_queue(int operation, std::vector<std::string> row)
{
    std::lock_guard<std::mutex> lock(*mut);
    if (operation == 0)
    {
        std::vector<std::string> ans = my_queue.front();
        my_queue.pop();

        return ans;
    }
    else if (operation == 1)
    {
        my_queue.push(row);
    }
    std::vector<std::string> dummy;
    return dummy;
}