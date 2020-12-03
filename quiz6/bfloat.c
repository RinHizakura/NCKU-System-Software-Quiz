#include <stdio.h>
float fp32tobf16(float x)
{
    float y = x;
    int *py = (int *) &y;
    unsigned int exp, man;
    exp = *py & 0x7F800000u;
    man = *py & 0x007FFFFFu;
    if (!exp && !man) /* zero */
        return x;
    if (exp == 0x7F800000u) /* infinity or NaN */
        return x;

    /* Normalized number. round to nearest */
    float r = x;
    int *pr = (int *) &r;
    *pr &= 0x7f800000;
    r /= 256;
    y = x + r;

    *py &= 0xffff0000;
    return y;
}
void print_hex(float x)
{
    int *p = (int *) &x;
    printf("%f=%x\n", x, *p);
}

int main()
{
    float a[] = {3.140625, 1.2, 2.31, 3.46, 5.63};
    for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
        print_hex(a[i]);
        float bf_a = fp32tobf16(a[i]);
        print_hex(bf_a);
    }

    return 0;
}
