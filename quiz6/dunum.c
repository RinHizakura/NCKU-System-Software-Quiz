#include <stdio.h>

int findDuplicate(int *nums, int numsSize)
{
    int res = 0;
    /* how many bits needed to represent*/
    const size_t log_2 = 8 * sizeof(int) - __builtin_clz(numsSize);
    for (size_t i = 0; i < log_2; i++) {
        int bit = 1 << i;
        int c1 = 0, c2 = 0;
        for (int k = 0; k < numsSize; k++) {
            if (k & bit)
                ++c1;
            if (nums[k] & bit)
                ++c2;
        }
        if (c1 < c2)
            res += bit;
    }
    return res;
}

#define XORSWAP_UNSAFE(a, b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))

int find(int *nums, int numsSize)
{
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] != i) {
            if (nums[nums[i]] == nums[i]) {
                return nums[i];
            } else {
                int index = nums[i];
                XORSWAP_UNSAFE(nums[i], nums[index]);
                i--;
            }
        }
    }

    return -1;
}

int main()
{
    int a[] = {1, 2, 2, 3, 4, 5};
    printf("%d %d\n", findDuplicate(a, 3), find(a, 3));
}
