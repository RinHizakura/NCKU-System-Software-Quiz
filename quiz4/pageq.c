#include <stdint.h>
#include <stdio.h>
#define ffs32(x) ((__builtin_ffs(x)) - 1)

int main()
{
    size_t offset = 123;

    for (int i = 1; i < 9; i++) {
        for (int n = 0; n < 15; n++) {
            if (i < 5 && n > 0)
                break;
            else if (i > 5 && n == 0)
                continue;
            int PAGE_QUEUES = i * (1 << n);
            size_t blockidx;
            size_t divident = PAGE_QUEUES;
            blockidx = offset >> ffs32(divident);
            divident >>= ffs32(divident);

            printf("%d: %ld\n", PAGE_QUEUES, divident);
            switch (divident) {
            case 3:
                blockidx /= 3;
                break;
            case 5:
                blockidx /= 5;
                break;
            case 7:
                blockidx /= 7;
                break;
            }

            if (blockidx != offset / PAGE_QUEUES)
                printf("%ld %ld\n", blockidx, offset / PAGE_QUEUES);
        }
    }
}
