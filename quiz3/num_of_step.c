#include <stdint.h>
#include <stdio.h>

int numberOfSteps(int num)
{
    return num ? __builtin_popcount(num) + 31 - __builtin_clz(num) : 0;
}

int main()
{
    printf("%d\n", numberOfSteps(5));
    return 0;
}
