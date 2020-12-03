#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v)
        return u | v;
    while (v) {
        uint64_t t = v;
        v = u % v;
        u = t;
    }
    return u;
}


uint64_t fast_gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v)
        return u | v;
    int shift;

    for (shift = 0; !((u | v) & 1); shift++) {
        u >>= 1, v >>= 1;
    }
    while (!(u & 1))
        u >>= 1;
    do {
        while (!(v & 1))
            v >>= 1;
        if (u < v) {
            v -= u;
        } else {
            uint64_t t = u - v;
            u = v;
            v = t;
        }
    } while (v);
    return u << shift;
}

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

uint64_t faster_gcd64(uint64_t u, uint64_t v)
{
    if (!u || !v)
        return u | v;

    int shift = min(__builtin_ctz(u), __builtin_ctz(v));
    u >>= shift;
    v >>= shift;

    while (!(u & 1))
        u >>= 1;
    do {
        while (!(v & 1))
            v >>= 1;
        if (u < v) {
            v -= u;
        } else {
            uint64_t t = u - v;
            u = v;
            v = t;
        }
    } while (v);
    return u << shift;
}



int main()
{
    printf("index,ctz,time(ns),label\n");

    for (int round = 0; round < 1000; round++) {
        for (int i = 0; i < 31; i++) {
            uint64_t a = (rand() << i) | (1 << i);
            uint64_t b = (rand() << i) | (1 << i);

            for (int label = 0; label < 2; label++) {
                struct timespec tt1, tt2;
                if (!label) {
                    clock_gettime(CLOCK_MONOTONIC, &tt1);
                    fast_gcd64(a, b);
                    clock_gettime(CLOCK_MONOTONIC, &tt2);
                } else {
                    clock_gettime(CLOCK_MONOTONIC, &tt1);
                    faster_gcd64(a, b);
                    clock_gettime(CLOCK_MONOTONIC, &tt2);
                }

                long long time = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                                 (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

                if (!label)
                    printf("%d,%d,%lld,fast_gcd\n", round, i, time);
                else
                    printf("%d,%d,%lld,faster_gcd\n", round, i, time);
            }
        }
    }
    return 0;
}
