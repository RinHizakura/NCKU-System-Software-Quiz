#include <assert.h>
#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PACKED_BYTE(b) (((uint64_t)(b) & (0xff)) * 0x0101010101010101u)

uint8_t hexchar2val(uint8_t in)
{
    const uint8_t letter = in & 0x40;
    const uint8_t shift = (letter >> 3) | (letter >> 6);
    return (in + shift) & 0xf;
}

uint64_t hexspeak(char str[])
{
    int len = strlen(str);
    assert(len <= 18);
    assert(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'));

    uint8_t tmp[8] = {
        0,
    };


#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int count = 0;
    for (int i = len - 1; i >= 2; i--) {
        if ((len - i) & 1)
            tmp[count] = hexchar2val(str[i]);
        else
            tmp[count++] += hexchar2val(str[i]) << 4;
    }
#else
    unsigned int count = 7;
    for (int i = len - 1; i >= 2; i--) {
        if ((len - i) & 1)
            tmp[count] = hexchar2val(str[i]);
        else
            tmp[count--] += hexchar2val(str[i]) << 4;
    }

#endif

    uint64_t *ans = (uint64_t *) tmp;
    return *ans;
}

int main()
{
    char a[] = "0xDEADBEAF";
    printf("%ld \n", hexspeak(a));

    return 0;
}
