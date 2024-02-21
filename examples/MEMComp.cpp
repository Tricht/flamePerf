#include <iostream>
#include <vector>
#include <memory>

// Ineffiziente Funktion: Kopiert eine große Datenmenge
void inefficientMemoryUse() {
    std::vector<int> largeData(10000000, 42);
    std::vector<int> copiedData = largeData; // Kopiert die Daten
    std::cout << "Ineffizient: Daten kopiert." << std::endl;
}

// Effizientere Funktion: Verwendet einen shared pointer zum Teilen der Daten
void efficientMemoryUse() {
    auto sharedData = std::make_shared<std::vector<int>>(10000000, 42);
    auto dataReference = sharedData; // Teilt die Daten ohne Kopie
    std::cout << "Effizient: Daten geteilt über Shared Pointer." << std::endl;
}

int main() {
    inefficientMemoryUse(); // Demonstriert ineffiziente Speichernutzung
    efficientMemoryUse();   // Demonstriert effiziente Speichernutzung
    return 0;
}
