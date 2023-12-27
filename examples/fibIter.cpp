#include <iostream>
#include <unordered_map>

unsigned long long fibIter(int n)
{
    std::unordered_map<int, unsigned long long> memo;

    memo[0] = 0;
    memo[1] = 1;

    for (int i = 2; i <= n; ++i)
    {
        memo[i] = memo[i - 1] + memo[i - 2];
    }

    return memo[n];
}

int main()
{
    int n = 100;
    std::cout << "Fibonacci number at position " << n << ": " << fibIter(n) << std::endl;

    return 0;
}
