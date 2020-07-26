#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <mutex>
#include "Producer.h"



int main()
{
    Producer main_thread;
    main_thread.set_data_directory("dataset/kdd.csv");
    main_thread.start();

    return 0;
}