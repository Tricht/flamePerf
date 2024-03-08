#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <unistd.h>

void intensiveComputation()
{
    for (long i = 0; i < 10000; ++i)
    {
        std::vector<int> temp(100, i);
    }
}

void sleepFunction()
{
    sleep(10);
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    intensiveComputation();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Computation duration "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    sleepFunction();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Sleep Function duration "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}
