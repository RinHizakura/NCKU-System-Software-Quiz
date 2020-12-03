#include <emmintrin.h>
#include <nmmintrin.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PACKED_BYTE(b) (((uint64_t)(b) & (0xff)) * 0x0101010101010101u)
#define SPACE 32
void print_m128i(__m128i var)
{
    uint64_t val[2];
    memcpy(val, &var, sizeof(val));
    printf("%lx %lx\n", val[0], val[1]);
}

uint64_t get_m128i_mask(__m128i var)
{
    // Here we assume var's last half bits are not needed
    uint64_t val[2];
    memcpy(val, &var, sizeof(val));

    return val[0];
}

bool is_valid_sse4(const char str[], size_t size)
{
    if (size == 0)
        return false;

    const __m128i ranges = _mm_setr_epi8('a', 'z', 'A', 'Z', '?', '?', ',', '.',
                                         SPACE, '!', 0, 0, 0, 0, 0, 0);
    const __m128i diff = _mm_set1_epi8(0xFF);

    const uint8_t mode = _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK;

    unsigned int i = 0;
    while ((i + 8) <= size) {
        uint64_t payload;
        memcpy(&payload, str + i, 8);
        uint64_t *payload_ptr = &payload;
        __m128i *mem = (__m128i *) (payload_ptr);

        const __m128i chunk = _mm_loadu_si128(mem);
        const __m128i mask = _mm_cmpestrm(ranges, 10, chunk, 8, mode);

        if (get_m128i_mask(_mm_xor_si128(mask, diff)))
            return false;

        i += 8;
    }

    while (i < size) {
        /* 1. lower
         * 2. upper
         * 3. ?
         * 4. , - .
         * 5. {space} ! */
        if (!((str[i] >= 65 && str[i] <= 90) ||
              (str[i] >= 97 && str[i] <= 122) || str[i] == 63 ||
              (str[i] >= 44 && str[i] <= 46) || (str[i] >= 32 && str[i] <= 33)))
            return false;
        i++;
    }
    return true;
}
bool is_ascii(const char str[], size_t size)
{
    if (size == 0)
        return false;
    unsigned int i = 0;
    while ((i + 8) <= size) {
        uint64_t payload;
        memcpy(&payload, str + i, 8);

        if (payload & PACKED_BYTE(0x80)) {
            return false;
        }
        i += 8;
    }
    while (i < size) {
        if (str[i] & 0x80)
            return false;
        i++;
    }
    return true;
}

bool is_alpha(const char str[], size_t size)
{
    if (size == 0)
        return false;
    unsigned int i = 0;
    while ((i + 8) <= size) {
        uint64_t payload;
        memcpy(&payload, str + i, 8);

        uint64_t A = payload + PACKED_BYTE(128 - 'A');
        uint64_t Z = payload + PACKED_BYTE(128 - 'Z' - 1);
        uint64_t a = payload + PACKED_BYTE(128 - 'a');
        uint64_t z = payload + PACKED_BYTE(128 - 'z' - 1);

        uint64_t lower_mask = (a ^ z) & PACKED_BYTE(0x80);
        uint64_t upper_mask = (A ^ Z) & PACKED_BYTE(0x80);

        if ((lower_mask | upper_mask) ^ PACKED_BYTE(0x80)) {
            return false;
        }
        i += 8;
    }

    while (i < size) {
        if ((str[i] < 65 || (str[i] > 90 && str[i] < 97) || str[i] > 122))
            return false;
        i++;
    }
    return true;
}


int main()
{
    char str[] = "This is a e-Book, huh?! Say.";
    printf("ans: %d\n", is_valid_sse4(str, strlen(str)));
    return 0;
}
