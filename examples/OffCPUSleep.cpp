#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

void intensiveComputation()
{
    for (long i = 0; i < 10000; ++i)
    {
        std::vector<int> temp(100, i);
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void sleepFunction()
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    intensiveComputation();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Computation duration "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    /*     start = std::chrono::high_resolution_clock::now();
        sleepFunction();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "Sleep Function duration "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

        start = std::chrono::high_resolution_clock::now();
        intensiveComputation();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "Computation duration "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl; */

    return 0;
}
