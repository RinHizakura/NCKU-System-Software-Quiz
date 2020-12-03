#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

/* A union allowing us to convert between a float and a 32-bit integers.*/
typedef union {
    float value;
    uint32_t v_int;
} ieee_float_shape_type;

/* Set a float from a 32 bit int. */
#define INSERT_WORDS(d, ix0)        \
    do {                            \
        ieee_float_shape_type iw_u; \
        iw_u.v_int = (ix0);         \
        (d) = iw_u.value;           \
    } while (0)

/* Get a 32 bit int from a float. */
#define EXTRACT_WORDS(ix0, d)       \
    do {                            \
        ieee_float_shape_type ew_u; \
        ew_u.value = (d);           \
        (ix0) = ew_u.v_int;         \
    } while (0)

static const float one = 1.0, tiny = 1.0e-30;

float ieee754_sqrt(float x)
{
    float z;
    int32_t sign = 0x80000000;
    uint32_t r;
    int32_t ix0, s0, q, m, t, i;

    EXTRACT_WORDS(ix0, x);

    /* take care of zero */
    if (ix0 <= 0) {
        if ((ix0 & (~sign)) == 0)
            return x; /* sqrt(+-0) = +-0 */
        if (ix0 < 0)
            return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
    }
    /* take care of +INF and NaN */
    if ((ix0 & 0x7f800000) == 0x7f800000) {
        /* sqrt(NaN) = NaN, sqrt(+INF) = +INF,*/
        return x;
    }
    /* normalize x */
    m = (ix0 >> 23);
    if (m == 0) { /* subnormal x */
        for (i = 0; (ix0 & 0x00800000) == 0; i++)
            ix0 <<= 1;
        m -= i - 1;
    }
    m -= 127; /* unbias exponent */
    ix0 = (ix0 & 0x007fffff) | 0x00800000;
    if (m & 1) { /* odd m, double x to make it even */
        ix0 <<= 1;
    }
    m >>= 1; /* m = [m/2] */


    /* generate sqrt(x) bit by bit */
    ix0 <<= 1;
    q = s0 = 0;     /* [q] = sqrt(x) */
    r = 0x01000000; /* r = moving bit from right to left */

    while (r != 0) {
        t = s0 + r;      // t = si + 2^(-i-1)
        if (t <= ix0) {  // t <= yi ?
            s0 = t + r;  // si+1 = si + 2^(-i)
            ix0 -= t;    // yi+1 = yi - t
            q += r;      // qi+1 = qi+ 2^(-i-1)
        }
        ix0 <<= 1;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if (ix0 != 0) {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one) {
            z = one + tiny;
            if (z > one)
                q += 2;
            else
                q += (q & 1);
        }
    }


    ix0 = (q >> 1) + 0x3f000000;
    ix0 += (m << 23);

    INSERT_WORDS(z, ix0);

    return z;
}



int mySqrt(int n)
{
    int32_t sign = 0x80000000;
    int32_t ix0, m, i;

    float x = n;
    EXTRACT_WORDS(ix0, x);

    /* take care of zero */
    if (ix0 <= 0) {
        if ((ix0 & (~sign)) == 0)
            return x; /* sqrt(+-0) = +-0 */
        if (ix0 < 0)
            return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
    }
    /* take care of +INF and NaN */
    if ((ix0 & 0x7f800000) == 0x7f800000) {
        /* sqrt(NaN) = NaN, sqrt(+INF) = +INF,*/
        return x;
    }
    /* normalize x */
    m = (ix0 >> 23);
    if (m == 0) { /* subnormal x */
        for (i = 0; (ix0 & 0x00800000) == 0; i++)
            ix0 <<= 1;
        m -= i - 1;
    }
    m -= 127; /* unbias exponent */
    ix0 = (ix0 & 0x007fffff) | 0x00800000;
    if (m & 1) { /* odd m, double x to make it even */
        ix0 += ix0;
    }
    m >>= 1; /* m = [m/2] */

    // binary search 'm'
    unsigned int head = 1 << m;        // 2^m
    unsigned int tail = 1 << (m + 1);  // 2^(m+1)
    if (m >= 15)
        tail = (1 << 16) - 1;
    unsigned int mid = (head + tail) >> 1;  // same as (2^m + 2^(m+1)) / 2

    while (1) {
        if ((unsigned int) n > (mid + 1) * (mid + 1)) {
            head = mid;
            mid = (head + tail) >> 1;
        } else if ((unsigned int) n < mid * mid) {
            tail = mid;
            mid = (head + tail) >> 1;
        } else
            break;
    }
    return mid;
}

int mySqrt_newton(int n)
{
    if (n == 0)
        return 0;
    if (n < 4)
        return 1;

    unsigned int ans = n / 2;

    if (ans > 65535)  // 65535 = 2^16 - 1
        ans = 65535;

    while (ans * ans > (unsigned int) n ||
           (ans + 1) * (ans + 1) <= (unsigned int) n) {
        ans = (ans + n / ans) / 2;
    }
    return ans;
}

int main()
{
    struct timespec tt1, tt2;
    for (int i = 1000000; i < 10000000; i += 100) {
        int ans1, ans2;
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        ans1 = mySqrt(i);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        long long time1 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        ans2 = mySqrt_newton(i);
        clock_gettime(CLOCK_MONOTONIC, &tt2);

        long long time2 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);
        printf("%d, %d, %d, %lld, %lld\n", i, ans1, ans2, time1, time2);
    }
    // printf("%f\n", ieee754_sqrt(7.0));
    return 0;
}
