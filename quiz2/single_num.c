#include <stdio.h>
#include <string.h>

int singleNumber(int *nums, int numsSize)
{
    int lower = 0, higher = 0;
    for (int i = 0; i < numsSize; i++) {
        lower ^= nums[i];
        lower &= ~higher;
        higher ^= nums[i];
        higher &= ~lower;
    }
    return lower;
}

int single_num(int *nums, int numsSize)
{
    int bit_count = 0;

    for (int i = 0; i < 32; i++) {
        int bit = 0;
        for (int idx = 0; idx < numsSize; idx++) {
            bit += (nums[idx] >> i) & 1;
        }
        bit_count |= (unsigned int) (bit % 3) << i;
    }


    return bit_count;
}

int main()
{
    int a[] = {-2, -2, 1, 1, -3, 1, -3, -3, -4, -2};
    single_num(a, 10);
    printf("%d \n", singleNumber(a, 10));
    return 0;
}
