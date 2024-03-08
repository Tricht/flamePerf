#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex1, mutex2;

void ThreadA()
{
    std::lock_guard<std::mutex> lock1(mutex1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> lock2(mutex2);

    std::cout << "Thread A\n";
}

void ThreadB()
{
    std::lock_guard<std::mutex> lock2(mutex2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard<std::mutex> lock1(mutex1);

    std::cout << "Thread B\n";
}

int main()
{
    std::thread threadA(ThreadA);
    std::thread threadB(ThreadB);

    threadA.join();
    threadB.join();

    return 0;
}
