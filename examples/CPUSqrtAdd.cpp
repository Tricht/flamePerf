#include <iostream>
#include <vector>
#include <cmath>

void intensiveComputation()
{
    double result = 0.0;
    for (unsigned int i = 1; i <= 10000000; i++)
    {
        result += std::sqrt(i);
    }
    std::cout << "Result first computation: " << result << std::endl;
}

void nonIntensiveComputation()
{
    double result = 0.0;
    for (unsigned int i = 1; i <= 10000000; i++)
    {
        result += i;
    }
    std::cout << "Result second computation: " << result << std::endl;
}

int main()
{
    std::cout << "Start computation" << std::endl;

    intensiveComputation();
    nonIntensiveComputation();

    std::cout << "End of Computation" << std::endl;
    return 0;
}
