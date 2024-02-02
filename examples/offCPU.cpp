#include <iostream>
#include <thread>
#include <chrono>

void simulateLongRunningTask() {
    std::this_thread::sleep_for(std::chrono::seconds(10)); // Simuliert eine lange Aufgabe
}

int main() {
    std::cout << "Simulate a long-running task" << std::endl;
    simulateLongRunningTask();
    std::cout << "Task completed" << std::endl;
    return 0;
}
