//#define HASH_SUBSTRING_A
#ifdef HASH_SUBSTRING_A

/* Find pattern in text */

/* Uses loop unrolling in hash(), precomputeHashes() and RabinKarp().
It uses three different ways of loop unrolling.
Other than that, it's the same as the original.
Well, actually, I had to change boundaries for some of the loops,
so they are unsigned, instead of signed (as in the original). */

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
    
    /* mod is slen % 8 - we'll unroll loop 8 times */
    size_t mod = slen & 7u;
    
    /* My way of unrolling a loop */

    register int i = slen - mod;

    /* This switch-case can be written as a simple for loop. */
    switch (mod) {
    case 7: h = (h + s[i+6]) % PRIME;
    case 6: h = (h + s[i+5]) % PRIME;
    case 5: h = (h + s[i+4]) % PRIME;
    case 4: h = (h + s[i+3]) % PRIME;
    case 3: h = (h + s[i+2]) % PRIME;
    case 2: h = (h + s[i+1]) % PRIME;
    case 1: h = (h + s[i]) % PRIME;
    case 0: ;
    }

    slen -= mod;
    for (i = slen - 1; i > -1; i -= 8) {
        h = (h + s[i]) % PRIME;                                 // h = (h * x + s[i]) % PRIME; in general case
        h = (h + s[i-1]) % PRIME;
        h = (h + s[i-2]) % PRIME;
        h = (h + s[i-3]) % PRIME;
        h = (h + s[i-4]) % PRIME;
        h = (h + s[i-5]) % PRIME;
        h = (h + s[i-6]) % PRIME;
        h = (h + s[i-7]) % PRIME;
    }

    return h;
}

ull *precomputeHashes(char *T, size_t lenT, size_t lenP) {
    ull *H = NULL;
    H = malloc((lenT - lenP + 1) * sizeof(*H));

    H[lenT - lenP] = hash(T + lenT - lenP);                     // T + lenT - lenP <==> &T[lenT - lenP], but it's probably faster.
    ull y = 1;

    register size_t i;                                          // Here, i doesn't have to be signed for the second loop, because we modified the indices in the second loop, and range of i.
    register size_t n;

//#define FIRST_LOOP
#ifdef FIRST_LOOP
    /* This loop is unnecessary when x == 1. */
    /* https://en.wikipedia.org/wiki/Duff%27s_device */
    /* This particular loop doesn't make use of i, so we can use Duff's device on it easily.
    count == lenP */
    n = (lenP + 7) >> 3;
    switch (lenP & 7u) {
    case 0: do { y = y % PRIME;                                 // y = (y * x) % PRIME; in general case
    case 7:      y = y % PRIME;
    case 6:      y = y % PRIME;
    case 5:      y = y % PRIME;
    case 4:      y = y % PRIME;
    case 3:      y = y % PRIME;
    case 2:      y = y % PRIME;
    case 1:      y = y % PRIME;
            } while (--n > 0);
    }
#endif // FIRST_LOOP

    /* https://en.wikipedia.org/wiki/Duff%27s_device
    This is instead of:
    for (i = lenT - lenP; i > 0; --i)
        H[i-1] = (H[i] + T[i-1] - y*T[i - 1 + lenP]) % PRIME;   // x*H[i] in general case
    */

    size_t count = lenT - lenP;
    if (!count)
        return H;
    i = count;
    n = (count + 7) >> 3;
    switch (count & 7u) {
    case 0: do { H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;     // x*H[i] in general case
    case 7:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 6:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 5:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 4:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 3:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 2:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
    case 1:      H[i - 1] = (H[i] + T[i - 1] - y*T[i - 1 + lenP]) % PRIME; --i;
            } while (--n > 0);
    }
    
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

    /* https://en.wikipedia.org/wiki/Loop_unrolling#C_example
    Modified the second part - using another for loop instead of switch-case,
    because it's a lot eaiser in this case, and probably about the same speed.
    We'll unroll the loop 8 times. */

    register size_t i = 0;
    size_t count = lenT - lenP + 1;
    size_t repeat = count >> 3;
    size_t mod = count & 7u;

    while (repeat--) {
        if (pHash == H[i] && !strncmp(T + i, P, lenP)) printf("%u ", i);    // T + i <==> &T[i], but probably faster.
        if (pHash == H[i+1] && !strncmp(T + i + 1, P, lenP)) printf("%u ", i + 1);
        if (pHash == H[i+2] && !strncmp(T + i + 2, P, lenP)) printf("%u ", i + 2);
        if (pHash == H[i+3] && !strncmp(T + i + 3, P, lenP)) printf("%u ", i + 3);
        if (pHash == H[i+4] && !strncmp(T + i + 4, P, lenP)) printf("%u ", i + 4);
        if (pHash == H[i+5] && !strncmp(T + i + 5, P, lenP)) printf("%u ", i + 5);
        if (pHash == H[i+6] && !strncmp(T + i + 6, P, lenP)) printf("%u ", i + 6);
        if (pHash == H[i+7] && !strncmp(T + i + 7, P, lenP)) printf("%u ", i + 7);

        i += 8;
    }

    for (; i < count; i++)
        if (pHash == H[i] && !strncmp(T + i, P, lenP)) printf("%u ", i);

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

#endif // HASH_SUBSTRING_A