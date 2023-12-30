#include <iostream>
#include <thread>
#include <vector>

void do_work() {
    // Simulierte Arbeit: eine einfache Schleife
    for (long i = 0; i < 100000000; ++i) {}
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(do_work);
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
