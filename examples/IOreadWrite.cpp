#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

void writeToDevNull() {
    std::ofstream devNull("/dev/null");
    if (!devNull.is_open()) {
        std::cerr << "Fehler beim Öffnen von /dev/null" << std::endl;
        return;
    }
    for (int i = 0; i < 1000000; ++i) {
        devNull << "Schreiben in /dev/null" << std::endl;
    }
}

void readFromDevZero() {
    std::ifstream devZero("/dev/zero");
    if (!devZero.is_open()) {
        std::cerr << "Fehler beim Öffnen von /dev/zero" << std::endl;
        return;
    }
    char buffer[1024];
    for (int i = 0; i < 1000000; ++i) {
        devZero.read(buffer, sizeof(buffer));
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    writeToDevNull();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Schreiben in /dev/null dauerte " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    readFromDevZero();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Lesen von /dev/zero dauerte " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}
