#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <mutex>
#include "Producer.hpp"



int main()
{
    // Create Producer object, assign dataset and begin interaction with user.
    // Note: Creating a Prodcuer automatically starts Consumer threads.
    Producer main_thread;
    
    main_thread.set_data_directory("dataset/kdd.csv");
    main_thread.start();

    return 0;
}