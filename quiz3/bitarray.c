#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
size_t naive(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    for (size_t k = 0; k < bitmapsize; ++k) {
        uint64_t bitset = bitmap[k];
        size_t p = k * 64;
        for (int i = 0; i < 64; i++) {
            if ((bitset >> i) & 0x1)
                out[pos++] = p + i;
        }
    }
    return pos;
}

size_t improved(uint64_t *bitmap, size_t bitmapsize, uint32_t *out)
{
    size_t pos = 0;
    uint64_t bitset;
    for (size_t k = 0; k < bitmapsize; ++k) {
        bitset = bitmap[k];

        while (bitset != 0) {
            uint64_t t = bitset & -bitset;
            int r = __builtin_ctzll(bitset);
            out[pos++] = k * 64 + r;
            bitset ^= t;
        }
    }
    return pos;
}

int main()
{
    uint64_t bitmap[4] = {
        0xFFFF0000FFFF0000,
        0xFFFF0000FFFF0000,
        0xFFFF0000FFFF0000,
    };

    uint32_t out[64 * 4];
    size_t pos = improved(bitmap, 4, out);
    printf("%ld \n", pos);

    for (int i = 0; i < (int) pos; i++) {
        printf("%d ", out[i]);
    }
    printf("\n");
}
