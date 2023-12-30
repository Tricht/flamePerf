#include <iostream>

unsigned long long fibRec(unsigned int n)
{
    if (n <= 1)
    {
        return n;
    }
    else
    {
        return fibRec(n - 1) + fibRec(n - 2);
    }
}

int main()
{
    int n = 50;
    std::cout << "Fibonacci number at position " << n << ": " << fibRec(n) << std::endl;

    return 0;
}