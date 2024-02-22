#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

void writeToFile()
{
    std::ofstream outFile("output.txt");
    for (int i = 0; i < 100000; ++i)
    {
        outFile << "Schreiben in Datei" << std::endl;
    }
}

void readFromFile()
{
    std::ifstream inFile("output.txt");
    char buffer[1];
    while (inFile.read(buffer, sizeof(buffer)))
    {
        std::string a = "";
        a += buffer;
    }
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    writeToFile();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Schreiben in Datei dauerte "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    readFromFile();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Lesen aus Datei dauerte "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;

    return 0;
}
