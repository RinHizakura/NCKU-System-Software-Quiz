#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

double divop(double orig, int slots)
{
    if (slots == 1 || orig == 0)
        return orig;
    int od = slots & 1;
    double result = divop(orig / 2, od ? (slots + 1) >> 1 : slots >> 1);
    if (od)
        result += divop(result, slots);
    return result;
}


int main()
{
    printf("%f %f\n", divop(3.7, 6));
    return 0;
}
