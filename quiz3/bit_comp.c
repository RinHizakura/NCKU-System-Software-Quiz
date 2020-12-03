#include <stdio.h>

int bitwiseComplement(int N)
{
    if (N == 0)
        return 1;

    int mask = (1 << (31 - __builtin_clz(N))) - 1;
    return ~N & mask;
}

int main()
{
    for (int i = 0; i < 10; i++)
        printf("%d %d\n", i, bitwiseComplement(i));

    return 0;
}
