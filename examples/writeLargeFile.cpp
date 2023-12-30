#include <iostream>
#include <fstream>
#include <string>

void write_read_large_file() {
    std::ofstream file("large_file.txt");
    for (int i = 0; i < 10000000; ++i) {
        file << "Test I/O" << std::endl;
    }
    file.close();

    std::ifstream infile("large_file.txt");
    std::string line;
    while (std::getline(infile, line)) {}
    infile.close();
}

int main() {
    std::cout << "write and read large file..." << std::endl;
    write_read_large_file();
    return 0;
}
