#define HASH_SUBSTRING
#ifdef HASH_SUBSTRING

/* Find pattern in text */

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

#define MAX_NUM_ITEMS 500001                                    // Max lengthf of P and T. +1 for '\0'.
#define MAX_P_OCCURENCES_LEN 100000000
#define TRUE 1
#define FALSE 0
#define PRIME 1000000007u                                       // This is a good choice.
//#define PRIME 2305843009213693951llu                          // This one also works fine, at least with x == 1 and x == 32. This is the first prime that is equal a power of two minus one that is larger than 10**14 + 31.
#define MR 1                                                    // Inclusive lower bound for x when generating it randomly.
#define NR PRIME - 1                                            // Inclusive upper bound for x when generating it randomly.

typedef unsigned long long ull;

/* x is used in hash() and precomputeHashes(), but I set x to 1 (in RabinKarp()), which is fine.
x should be in [1, PRIME - 1]. x == 1 is the fastest.
x can be chosen randomly in run-time, or hard-coded. */
size_t x;

ull hash(char *s) {
    ull h = 0;
    size_t slen = strlen(s);

    for (register int i = slen - 1; i > -1; --i)
        h = (h + s[i]) % PRIME;                                 // h = (h * x + s[i]) % PRIME; in general case

    return h;
}

ull *precomputeHashes(char *T, size_t lenT, size_t lenP) {
    ull *H = NULL;
    H = malloc((lenT - lenP + 1) * sizeof(*H));

    H[lenT - lenP] = hash(T + lenT - lenP);                     // T + lenT - lenP <==> &T[lenT - lenP], but it's probably faster.
    ull y = 1;

    /* This loop is unnecessary when x == 1. */
    for (size_t i = 1; i < lenP + 1; i++)
        y = y % PRIME;                                          // y = (y * x) % PRIME; in general case

    for (int i = lenT - lenP - 1; i > -1; --i)                  // i has to be signed here!
        H[i] = (H[i + 1] + T[i] - y*T[i + lenP]) % PRIME;       // x*H[i+1] in general case
        /* Instead of that, if needed, we can write:
        H[i] = (x*H[i + 1] + T[i] + PRIME - y*T[i + lenP]) % PRIME;
        or
        H[i] = (((x*H[i + 1] + T[i] - y*T[i + lenP]) % PRIME) + PRIME) % PRIME;
        But, the above works, and it's fastest.
        */

    /* This last loop can be rewritten as:
    for (i = lenT - lenP; i > 0; --i)
        H[i-1] = (H[i] + T[i-1] - y*T[i - 1 + lenP]) % PRIME;   // x*H[i] in general case
    In this case, type(i) can be size_t.
    */

    return H;
}

void RabinKarp(char *T, char *P) {
    size_t lenT = strlen(T);
    size_t lenP = strlen(P);
    //x = MR + rand() / (RAND_MAX / (NR - MR + 1) + 1);           // rand() returns int
    x = 1;                                                      // This is just fine. It's the fastest.
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
    /* Intializes random number generator */
    time_t t;
    srand((unsigned)time(&t));

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

#endif // HASH_SUBSTRING 