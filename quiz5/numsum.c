#include <stdio.h>

int consecutiveNumbersSum(int N)
{
    if (N < 1)
        return 0;
    int ret = 1;
    int x = N;
    for (int i = 2; i < x; i++) {
        x -= (i - 1);
        if (x % i == 0)
            ret++;
    }
    return ret;
}

int main()
{
    printf("%d\n", consecutiveNumbersSum(15));
    return 0;
}
