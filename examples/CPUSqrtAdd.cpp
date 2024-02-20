#include <iostream>
#include <vector>
#include <cmath>

void intensiveComputation() {
    double result = 0.0;
    for (unsigned int i = 1; i <= 10000000; i++) {
        result += std::sqrt(i);
    }
    std::cout << "Ergebnis der ersten Berechnung: " << result << std::endl;
}

void nonIntensiveComputation() {
    double result = 0.0;
    for (unsigned int i = 1; i <= 10000000; i++) {
        result += i;
    }
    std::cout << "Ergebnis der zweiten Berechnung: " << result << std::endl;
}

int main() {
    std::cout << "Start der Operationen..." << std::endl;

    intensiveComputation();
    nonIntensiveComputation();

    std::cout << "Operationen abgeschlossen." << std::endl;
    return 0;
}
