#include <stdio.h>

int hammingDistance(int x, int y)
{
    return __builtin_popcount(x ^ y);
}


int totalHammingDistance(int *nums, int numsSize)
{
    unsigned int hamming = 0;
    unsigned int bit = 0;

    if (numsSize < 2)
        return 0;

    for (int i = 0; i < 32; i++) {
        bit = 0;
        for (int j = 0; j < numsSize; j++) {
            if (nums[j] & 1)
                bit++;
            nums[j] >>= 1;
        }
        hamming += bit * (numsSize - bit);
    }

    return hamming;
}


int brute(int *nums, int numsSize)
{
    int hamming = 0;
    for (int i = 0; i < numsSize; i++)
        for (int j = i + 1; j < numsSize; j++)
            hamming += hammingDistance(nums[i], nums[j]);

    return hamming;
}



int main()
{
    int a[] = {4, 5, 6, 7, 8, 9, 10, 100, 120, 14, 2};

    printf("%d %d\n", totalHammingDistance(a, 11), brute(a, 11));

    return 0;
}
