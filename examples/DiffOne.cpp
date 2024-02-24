#include <iostream>

void add(int n)
{
    long sum = 0;
    for (int i = 0; i < n; ++i)
    {
        sum += i;
    }
    std::cout << "Sum: " << sum << std::endl;
}

void subtract(int n)
{
    long diff = 0;
    for (int i = 0; i < n; ++i)
    {
        diff -= i;
    }
    std::cout << "Diff: " << diff << std::endl;
}

void multiply(int n)
{
    long prod = 1;
    for (int i = 1; i <= n; ++i)
    {
        prod *= i;
    }
    std::cout << "Prod: " << prod << std::endl;
}

void divide(int n)
{
    long double div = 1.0;
    for (int i = 1; i <= n; ++i)
    {
        div /= i;
    }
    std::cout << "Div: " << div << std::endl;
}

int main()
{
    int n = 1000;

    add(n);
    subtract(n);
    multiply(n);
    divide(n);

    return 0;
}
