//#define HASH_SUBSTRING_ALT
#ifdef HASH_SUBSTRING_ALT

/* Find pattern in text */

/* Uses alternative implementations of hash() and precomputeHashes().
Namely, PRIME is chosen as the first larger power of two minus one, of
a previously selected prime, so that the functions can work faster
(at least in theory). Here, they don't use % operator, but
"bit twiddling hacks".
http://graphics.stanford.edu/~seander/bithacks.html#ModulusDivisionParallel */

/* PRIME == 1000000007u and (PRIME == 2305843009213693951llu with x == 32) give the same execution speed. */

/* Profiler reports that most of the time is spent printing results in RabinKarp(),
which makes sense to me. That's why results between different variants don't vary much.
Things like loop unrolling, loop interchange, hacks for modulo division, and similar
should be benchmarked in a large loop, with time measured inside of compilation unit,
without printing results of algorithms. */


#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NUM_ITEMS 500001                                    // Max lengthf of P and T. +1 for '\0'
#define MAX_P_OCCURENCES_LEN 100000000
#define TRUE 1
#define FALSE 0
#define MR 1                                                    // Inclusive lower bound for x when generating it randomly.
#define NR PRIME - 1                                            // Inclusive upper bound for x when generating it randomly.

//#define GIGANTIC_PRIME
#ifdef GIGANTIC_PRIME
#define PRIME 2305843009213693951llu                            // This is the first prime that is equal a power of two minus one that is larger than 10**14 + 31.
#define POWER 61                                                // PRIME == 2**POWER - 1
#define M 0x1fffffffffffffffllu
#define Q 61
#define R 0x1fffffffffffffffllu
#else
#define PRIME 2147483647u                                       // This is the first prime that is equal a power of two minus one that is larger than 10**9 + 7 (which works fine).
#define POWER 31                                                // PRIME == 2**POWER - 1
#define M 0x7fffffffu
#define Q 31
#define R 0x7fffffffu
#endif // GIGANTIC_PRIME

typedef unsigned long long ull;

/* x is used in hash() and precomputeHashes(), but I set x to 1 (in RabinKarp()), which is fine.
x should be in [1, PRIME - 1]. x == 1 is the fastest.
x can be chosen randomly in run-time, or hard-coded. */
size_t x;

/* Hash function for strings.
http://graphics.stanford.edu/~seander/bithacks.html#ModulusDivision
The three constants used in this function are what I think they should be. */
ull hash(char *s) {
    ull h = 0;
    size_t slen = strlen(s);

    for (register int i = slen - 1; i >= 0; --i) {
        h = h + s[i];                                           // h = h*x + s[i]; in general case, but h << 0 should be faster - this is like x is 1 (h = ((h << 0) + s[i]);)

        /* h % PRIME goes into m (modulus) */
        ull m;

        m = (h & M) + ((h >> POWER) & M);

        for ( ; m > PRIME; )
            m = (m >> Q) + (m & R);

        h = m == PRIME ? 0 : m;
    }

    return h;
}

ull *precomputeHashes(char *T, size_t lenT, size_t lenP) {
    ull *H = NULL;
    H = malloc((lenT - lenP + 1) * sizeof(*H));

    H[lenT - lenP] = hash(T + lenT - lenP);                     // T + lenT - lenP <==> &T[lenT - lenP], but it's probably faster.
    ull y = 1;

    /* q and r are left here as an example, but we use Q and R instead. */
    //static const size_t q = 61;
    //static const ull r = 0x1fffffffffffffffllu;

    /* y % PRIME or H[i] % PRIME go into m (modulus) */
    ull m;

    /* This loop is unnecessary when x == 1. */
    for (size_t i = 1; i < lenP + 1; i++) {
        //y <<= 0;                                                // y = y * x; in general case, but y << 0 should be faster - this is like x is 1 (y <<= 0;)

        m = (y & M) + ((y >> POWER) & M);

        for ( ; m > PRIME; )
            m = (m >> Q) + (m & R);

        y = m == PRIME ? 0 : m;
    }

    for (int i = lenT - lenP - 1; i > -1; --i) {                // i has to be signed here!
        H[i] = H[i + 1] + T[i] - y*T[i + lenP];                 // x*H[i+1] in general case
        /* Instead of that, if needed, we can write:
        H[i] = (x*H[i + 1] + T[i] + PRIME - y*T[i + lenP]) % PRIME;
        or
        H[i] = (((x*H[i + 1] + T[i] - y*T[i + lenP]) % PRIME) + PRIME) % PRIME;
        But, the above works, and it's fastest.
        */

        m = (H[i] & M) + ((H[i] >> POWER) & M);

        for ( ; m > PRIME; )
            m = (m >> Q) + (m & R);

        H[i] = m == PRIME ? 0 : m;
    }

    return H;
}

void RabinKarp(char *T, char *P) {
    size_t lenT = strlen(T);
    size_t lenP = strlen(P);
    //x = MR + rand() / (RAND_MAX / (NR - MR + 1) + 1);           // rand() returns int, and N is huge, so x is int
    ull pHash = hash(P);
    ull *H = NULL;
    H = precomputeHashes(T, lenT, lenP);
    for (size_t i = 0; i < lenT - lenP + 1; i++) {
        if (pHash != H[i])
            continue;
        if (!strncmp(T + i, P, lenP))                           // T + i <==> &T[i], but probably faster.
            printf("%u ", i);
    }
    printf("\n");
    free(H);
}

int main(void) {
    static char pattern[MAX_NUM_ITEMS], text[MAX_NUM_ITEMS];

    scanf("%s", pattern);
    scanf("%s", text);

    RabinKarp(text, pattern);

    char c = getchar();
    c = getchar();
    return 0;
}

/* Test data:

We should input two strings, in two rows, and the result is
positions of occurences of the first string in the second one.

Input:
aba
abacaba
Output:
0 4

Input:
Test
testTesttesT
Output:
4

Input:
aaaaa
baaaaaaa
Output:
1 2 3
*/

#endif // HASH_SUBSTRING_ALT 