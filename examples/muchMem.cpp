#include <iostream>
#include <vector>

int main() {
    std::vector<int> large_vector;
    try {
        for (long i = 0; i < 1000000000; ++i) {
            large_vector.push_back(i);
        }
    } catch (std::bad_alloc &e) {
        std::cerr << "Speicherallokation fehlgeschlagen: " << e.what() << std::endl;
    }
    return 0;
}
