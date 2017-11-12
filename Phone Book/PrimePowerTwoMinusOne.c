//#define PRIME_POWER_TWO_MINUS_ONE
#ifdef PRIME_POWER_TWO_MINUS_ONE

/* Finds a prime that is equal a power of two minus one */

/* Finding such a prime can further improve speed of hashing. */

/* http://graphics.stanford.edu/~seander/bithacks.html#ModulusDivisionEasy and related following two algorithms.
Also: http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_NUM_ITEMS 100000
#define TRUE 1
#define FALSE 0

/* Calculates and returns the first larger power of two of the input number. */
unsigned long long calculateLargerPowerOfTwo(unsigned long long in) {
    unsigned long long out, inCopy = in;
    register unsigned int i;
    for (i = 0; inCopy > 0; i++) {
        inCopy >>= 1;
    }
    out = 1llu << i;
    return out;
}

/* Checks whether a number is prime. */
int isPrimeComplete(unsigned long long n) {
    if (n <= 1)
        return FALSE;
    else if (n <= 3)
        return TRUE;
    else if ((n % 2 == 0) || (n % 3 == 0))
        return FALSE;
    unsigned long long i = 5;
    while (i * i <= n) {
        if ((n % i == 0) || (n % (i + 2) == 0))
            return FALSE;
        i += 6;
    }
    return TRUE;
}

/* Checks whether a number > 3 is prime.
So, n must be > 3.
Skips checks for n <= 3. */
int isPrime(unsigned long long n) {
    if ((n % 2 == 0) || (n % 3 == 0))
        return FALSE;
    unsigned long long i = 5;
    while (i * i <= n) {
        if ((n % i == 0) || (n % (i + 2) == 0))
            return FALSE;
        i += 6;
    }
    return TRUE;
}

/* Checks whether an odd number > 3 is prime.
So, n must be > 3 and must be odd.
Skips checks for n <= 3 and n % 2. */
int isPrimeFast(unsigned long long n) {
    if (n % 3 == 0)
        return FALSE;
    unsigned long long i = 5;
    while (i * i <= n) {
        if ((n % i == 0) || (n % (i + 2) == 0))
            return FALSE;
        i += 6;
    }
    return TRUE;
}

/* lowerBound must be odd!
Returns a prime, with no special property. */
unsigned long long findPrime(unsigned long long lowerBound) {
    if (!(lowerBound % 2)) {
        printf("lowerBound must be odd!\n");
        return 0;
    }
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    for (unsigned long long n = lowerBound; n < upperBound; n += 2) {
        if (isPrime(n))
            return n;
    }
    return FALSE;
}

/* lowerBound must be odd!
Returns a prime that is equal a power of two minus one. */
unsigned long long findPrimePowerTwoMinusOneNaive(unsigned long long lowerBound) {
    if (!(lowerBound % 2)) {
        printf("lowerBound must be odd!\n");
        return 0;
    }
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    for (register unsigned long long n = lowerBound; n < upperBound; n += 2) {
        if (((n & (n - 1)) == 0) && isPrime(n - 1))             // We first check to see whether n is a power of two, and then whether n-1 is prime.
            return n - 1;
    }
    return FALSE;
}

/* lowerBound must be odd!
Returns a prime that is equal a power of two minus one. */
unsigned long long findPrimePowerTwoMinusOne(unsigned long long lowerBound) {
    if (!(lowerBound % 2)) {
        printf("lowerBound must be odd!\n");
        return 0;
    }
    register unsigned long long n = calculateLargerPowerOfTwo(lowerBound);
    for (n; ; n <<= 1) {
        if (isPrime(n - 1))                                     // We first check to see whether n is a power of two, and then whether n-1 is prime.
            return n - 1;
    }
    return FALSE;
}

/* Returns a prime, with no special property. */
unsigned long long findPrimeRandomly(unsigned long long lowerBound) {
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    const unsigned long long M = lowerBound, N = upperBound - 1;
    while (TRUE) {
        register unsigned long long n = M + rand() / (RAND_MAX / (N - M + 1) + 1);
        if (!(n % 2))
            continue;
        if (isPrime(n))
            return n;
    }
}

/* Returns a prime that is equal a power of two minus one. */
unsigned long long findPrimePowerTwoMinusOneRandomlyNaive(unsigned long long lowerBound) {
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    const unsigned long long M = lowerBound, N = upperBound - 1;
    while (TRUE) {
        register unsigned long long n = M + rand() / (RAND_MAX / (N - M + 1) + 1);
        if (!(n % 2))
            continue;
        if (((n & (n - 1)) == 0) && isPrime(n - 1))             // We first check to see whether n is a power of two, and then whether n-1 is prime.
            return n - 1;
    }
}

int main(void) {
    /* Intializes random number generator */
    time_t t;
    srand((unsigned)time(&t));

    printf("0, %d\n", isPrimeComplete(0));
    printf("1, %d\n", isPrimeComplete(1));
    printf("2, %d\n", isPrimeComplete(2));
    printf("3, %d\n", isPrimeComplete(3));
    printf("10, %d\n", isPrime(10));
    printf("13, %d\n", isPrime(13));
    printf("1000000007, %d\n", isPrime(1000000007));
    puts("");

    unsigned long long n;
    n = (unsigned long long)powl(10, 9) - 1;                    // not prime!
    printf("%llu, %d\n", n, isPrime(n));
    n = (unsigned long long)powl(10, 13) + 37;                  // prime!
    printf("%llu, %d\n", n, isPrime(n));
    n = (unsigned long long)powl(10, 13) + 38;                  // not prime!
    printf("%llu, %d\n", n, isPrime(n));
    n = (unsigned long long)powl(10, 14) + 31;                  // prime!
    printf("%llu, %d\n", n, isPrime(n));
    n = (unsigned long long)powl(10, 14) + 32;                  // not prime!
    printf("%llu, %d\n", n, isPrime(n));
    n = 2147483647;                                             // prime!
    printf("%llu, %d\n", n, isPrimeFast(n));
    n = 2305843009213693951;                                    // prime!
    printf("%llu, %d\n", n, isPrimeFast(n));
    puts("");

    n = 10000019;                                               // df = 18; findPrimePowerTwoMinusOne() returns prime = 2147483647 (10 digits; it's the largest possible signed 32-bit int) in 0.000s with /O2 & /Ot
    n = 1000000007;                                             // df = 6; findPrimePowerTwoMinusOne() returns prime =  2147483647
    n = (unsigned long long)powl(10, 13) + 37;                  // df = 36
    n = (unsigned long long)powl(10, 14) + 31;                  // df = 30
    const int df = 30;
    unsigned long long res;
    clock_t t0, t1;
    float diff;
    
    printf("findPrime()\n");
    t0 = clock();
    res = findPrime(n - df);                                    // 0.051 s, for prime 100000000000031 when using isPrimeComplete() (0.030 s with /O2 & /Ot); BUT 0.040 s when using isPrime() (0.030 s with /O2 & /Ot)
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);
    
    printf("findPrimeRandomly()\n");
    t0 = clock();
    res = findPrimeRandomly(n - df);                            // 0.046 s when using isPrimeComplete() (0.031 s with /O2 & /Ot); BUT 0.037 s when using isPrime() (0.030 s with /O2 & /Ot)
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);

#ifdef PAIN
    printf("findPrimePowerTwoMinusOneNaive()\n");
    t0 = clock();
    res = findPrimePowerTwoMinusOneNaive(n - df);               // way too slow... takes forever...
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);
#endif // PAIN

    printf("findPrimePowerTwoMinusOne()\n");
    t0 = clock();
    res = findPrimePowerTwoMinusOne(n - df);                    // 6.117 s for prime 2305843009213693951 (19 digits) when using isPrimeComplete() (4.908 s with /O2 & /Ot); 5.956 s when using isPrime() (4.854 s with /O2 & /Ot)
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);

#ifdef PAIN
    printf("findPrimePowerTwoMinusOneRandomlyNaive()\n");
    t0 = clock();
    res = findPrimePowerTwoMinusOneRandomlyNaive(n - df);       // way too slow... takes forever...
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);
#endif // PAIN

    char c = getchar();
    return 0;
}

#endif // PRIME_POWER_TWO_MINUS_ONE 