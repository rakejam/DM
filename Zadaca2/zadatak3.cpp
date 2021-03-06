#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstring>
#include <sstream>

using namespace std;

std::string to_string(long i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}


long long bin[100][100];

int binom(int n, int k)
{
    if (bin[n] [k] != -1)
        return bin[n][k];
    if (n == 0 || n == k)
        return bin[n][k] = 1;
    return bin[n][k] = binom(n - 1, k - 1) + binom(n - 1, k);

}

long long s[100][100];

long long s_recursive(long long n,long long k)
{
    if (s [n] [k] != -1)
        return s [n] [k];

    if (k == 1 || k == n)
        return s [n] [k] = 1;

    return s [n] [k] = s_recursive(n-1,k-1) + k*s_recursive(n-1,k);
}

long long c_over(long long n, long long k)
{
    return binom(n + k - 1, n);
}

string render_stirling(long long n, long long k)
{
    return "S_{" + to_string(n) + "}^{" + to_string(k) + "}";
}

int main()
{
    memset(s, -1, sizeof s);
    memset(bin, -1, sizeof bin);

    long long solution = 0;

    std::cout << c_over(6, 4) << ' ' << c_over(11, 4) << ' ' << c_over(11, 4) << ' ' << c_over(6, 4) * c_over(11, 4) * c_over(11, 4) << '\n';

    return 0;
}

