#include <stdbool.h>
#include <stdio.h>

bool isPowerOfFour(int num)
{
    return num > 0 && (num & (num - 1)) == 0 && !(__builtin_ctzl(num) & 1);
}

bool isPowerOfFour_A(int num)
{
    return !(__builtin_popcount(num) ^ 1) && __builtin_ffs(num) & 1;
}

bool isPowerOfFour_B(int num)
{
    int ffs = __builtin_ffs(num);
    return !(num >> ffs) && (ffs & 1);
}

int main()
{
    for (int i = 0; i < 100; i++)
        printf("%d %d %d %d\n", i, isPowerOfFour(i), isPowerOfFour_A(i),
               isPowerOfFour_B(i));

    return 0;
}
