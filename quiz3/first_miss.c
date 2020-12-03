#include <stdint.h>
#include <stdio.h>

#define XORSWAP_UNSAFE(a, b) ((a) ^= (b), (b) ^= (a), (a) ^= (b))
int firstMissingPositive(int *nums, int numsSize)
{
    for (int i = 0; i < numsSize; i++) {
        if (nums[i] > 0 && nums[i] < numsSize) {
            int pos = nums[i] - 1;
            if (pos != i && nums[i] != nums[pos]) {
                XORSWAP_UNSAFE(nums[i], nums[pos]);
                i--;
            }
        }
    }

    for (int i = 0; i < numsSize; i++) {
        if (nums[i] != i + 1)
            return i + 1;
    }

    return numsSize + 1;
}

int main()
{
    int a[] = {1, 1};
    int b[] = {1, 2, 3, 4, 5, 6};

    printf("%d \n", firstMissingPositive(a, 2));
    printf("%d \n", firstMissingPositive(b, 6));

    return 0;
}
