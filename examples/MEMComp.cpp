#include <iostream>
#include <vector>
#include <memory>

void inefficientMemoryUse()
{
    std::vector<int> largeData(10000000, 42);
    std::vector<int> copiedData = largeData;
    std::cout << "Data copied" << std::endl;
}

void efficientMemoryUse()
{
    auto sharedData = std::make_shared<std::vector<int>>(10000000, 42);
    auto dataReference = sharedData;
    std::cout << "Data shared with shared pointer" << std::endl;
}

int main()
{
    inefficientMemoryUse();
    efficientMemoryUse();
    return 0;
}
