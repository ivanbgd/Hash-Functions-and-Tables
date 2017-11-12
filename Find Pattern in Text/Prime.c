//#define PRIME
#ifdef PRIME

/* Finds a prime */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_NUM_ITEMS 100000
#define TRUE 1
#define FALSE 0

/* Checks whether a number is prime. */
int isPrime(unsigned long long n) {
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

/* lowerBound must be odd! */
unsigned long long findPrime(unsigned long long lowerBound) {
    if (!(lowerBound % 2)) {
        printf("lowerBound must be odd!\n");
        return 0;
    }
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    for (unsigned long long n = lowerBound; n < upperBound; n += 2) {
        if (isPrimeFast(n))
            return n;
    }
    return FALSE;
}

unsigned long long findPrimeRandomly(unsigned long long lowerBound) {
    unsigned long long upperBound = lowerBound << 1;            // According to Bertrand's postulate, this is certain upper bound.
    const unsigned long long M = lowerBound, N = upperBound - 1;
    while (TRUE) {
        unsigned long long n = M + rand() / (RAND_MAX / (N - M + 1) + 1);
        if (!(n % 2))
            continue;
        if (isPrimeFast(n))
            return n;
    }
}

int main(void) {
    /* Intializes random number generator */
    time_t t;
    srand((unsigned)time(&t));

    printf("0, %d\n", isPrime(0));
    printf("1, %d\n", isPrime(1));
    printf("2, %d\n", isPrime(2));
    printf("3, %d\n", isPrime(3));
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
    puts("");

    n = (unsigned long long)powl(10, 13) + 37;
    n = (unsigned long long)powl(10, 14) + 31;
    unsigned long long res;
    clock_t t0, t1;
    float diff;

    t0 = clock();
    res = findPrime(n - 30);                                    // 0.051 s, for prime 100000000000031 (0.030 s with /O2 & /Ot)
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);
    
    t0 = clock();
    res = findPrimeRandomly(n - 30);                            // 0.046 s (0.031 s with /O2 & /Ot)
    t1 = clock();
    diff = (float)(t1 - t0) / CLOCKS_PER_SEC;
    printf("%llu, %.3lf s\n", res, diff);

    char c = getchar();
    return 0;
}

#endif // PRIME 
