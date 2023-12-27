#include <iostream>

int fibRec(int n)
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
    int n = 10;
    std::cout << "Fibonacci number at position " << n << ": " << fibRec(n) << std::endl;

    return 0;
}