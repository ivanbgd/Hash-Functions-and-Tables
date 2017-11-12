//#define PHONE_BOOK_ALT
#ifdef PHONE_BOOK_ALT

/* Phone book alternative */

/* Alternative meaning uses hash table whose size iz power of two.
Rationale:
Andrew Bromage's answer at:
https://www.quora.com/What-are-some-things-low-level-programmers-know-but-high-level-ones-dont

"The reason why you'd use a prime-sized hash table was to cover up bad behaviour in your hash function. 
On a modern CPU, it is worth using a more sophisticated hash function built out of fully pipelineable operations 
(e.g. multiplies or even table look-ups) and using hash tables which are powers of two, so the modulo operation is a bit mask. 
Do anything to avoid that division operation." */

/* So, this alternative version should be faster. */

/* Both Singly and Doubly Linked Lists work. The only difference is in function erase().
Of course, Singly doesn't need the prev pointer.
They have the same performances (speed-wise and memory-wise). */

/* BUT, we should use printf() in processQueries(), or memcpy() or strcpy(), because strcat() version is too slow. */

/* Keep in mind that strcpy() and strcat() are not considered safe, because they can overflow the
destination buffer, and thus should be avoided.
https://en.wikipedia.org/wiki/C_standard_library#Buffer_overflow_vulnerabilities */


#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_ITEMS 100000
#define TRUE 1
#define FALSE 0
#define HASH_TABLE_SIZE 8192u                                   // == 1 << 13 == pow(2, 13) == 2 << 12 - unused
#define PRIME 10000019u
#define RATIO 1                                                 // ratio of numQueries (number of inputs) and number of buckets in the hash table - note that there can be many same inputs
#define MAX_NAME_LEN 16                                         // 15 + 1 for the terminating character


/* HASH TABLE CODE */

typedef struct Element Element;

struct Element {
    int number;
    char name[MAX_NAME_LEN];
    Element *prev, *next;
};

/* Hash function for integers.
Old-style hash(), the slowest variant.
Uses integer division (modulo), which is really slow.
Left here for historical and comparison reasons.
Incompatible with the rest of the code, since it takes hashTableSize
instead of mask, which all the other functions take. */
unsigned int hashSlowest(int x, int hashTableSize) {
    return (unsigned int)(((((unsigned int)x << 5) + 1) % PRIME) % hashTableSize);
}

/* Hash function for integers.
Adapted to work with hashTableSize which is exclusively power of two.
This is faster than hashSlowest(), because it uses & instead of %.
Left here for historical and comparison reasons.
Incompatible with the rest of the code, since it takes hashTableSize
instead of mask, which all the other functions take. */
unsigned int hashSlower(int x, int hashTableSize) {
    unsigned int mask = hashTableSize - 1;
    return (unsigned int)(((((unsigned int)x << 5) + 1) % PRIME) & mask);
}

/* Hash function for integers.
Adapted to work with hashTableSize which is exclusively power of two.
This is faster than hashSlower().
Further adapted to take mask instead of hashTableSize.
mask == hashTableSize - 1, but that has already been calculated, and only once,
so this is even faster.
All functions needed to change, though (except for freeHashTable()), to
also take mask instead of hashTableSize. */
unsigned int hash(int x, unsigned int mask) {
    return (unsigned int)(((((unsigned int)x << 5) + 1) % PRIME) & mask);
}

/* mask == hashTableSize - 1 (hashTableSize is number of buckets).
Private function. Used in insert(). */
Element *_find(Element **hashTable, unsigned int mask, int number) {
    Element *ep = NULL;                                         // pointer to an Element
    for (ep = hashTable[hash(number, mask)]; ep != NULL; ep = ep->next) {
        if (ep->number == number)
            return ep;                                          // found
    }
    return NULL;                                                // not found
}

/* mask == hashTableSize - 1 (hashTableSize is number of buckets).
Public function.
It would be more general to return Element* (ep or NULL),
but it's faster to return a string, because in this case,
we don't have to check whether the pointer is NULL or not
in the calling routine. */
char *find(Element **hashTable, unsigned int mask, int number) {
    Element *ep = NULL;                                         // pointer to an Element
    for (ep = hashTable[hash(number, mask)]; ep != NULL; ep = ep->next) {
        if (ep->number == number)
            return ep->name;                                    // found
    }
    return "not found";                                         // not found
}

/* Inserts an element if there's no element with the given number.
If there is the given number already, rewrites the element's name field.
mask == hashTableSize - 1 (hashTableSize is number of buckets).
Returns nothing. */
void insert(Element **hashTable, unsigned int mask, int number, char *name) {
    Element *ep = NULL;                                         // pointer to an Element
    unsigned int hashValue = 0;
    if (!(ep = _find(hashTable, mask, number))) {               // not found
        ep = malloc(sizeof(*ep));                               // sizeof(Element)
        if (!ep)                                                // if malloc fails
            exit(-1);
        hashValue = hash(number, mask);
        ep->number = number;
        strcpy(ep->name, name);
        ep->prev = NULL;                                        // this element will be the first one in the bucket
        ep->next = hashTable[hashValue];                        // always references the first element of the bucket (even if it's a NULL)
        if (ep->next)
            ep->next->prev = ep;
        hashTable[hashValue] = ep;                              // adds this pointer as the first one in the bucket
    }
    else {                                                      // already there
        strcpy(ep->name, name);
    }
}

/* Erases the element with the given number, if it exists.
If it doesn't exist, ignores the request. */
void eraseDoubly(Element **hashTable, unsigned int mask, int number) {
    Element *ep = NULL;
    if (!(ep = _find(hashTable, mask, number)))
        return;                                                 // not found
    if (!(ep->prev))
        hashTable[hash(number, mask)] = ep->next;
    else
        ep->prev->next = ep->next;
    if (ep->next)
        ep->next->prev = ep->prev;
    free(ep);
}

/* Erases the element with the given number, if it exists.
If it doesn't exist, ignores the request. */
void erase(Element **hashTable, unsigned int mask, int number) {
    Element *ep = NULL;
    if (!(ep = _find(hashTable, mask, number)))
        return;                                                 // not found
    Element *first = hashTable[hash(number, mask)];
    Element *epc = ep;
    if (first == ep)
        hashTable[hash(number, mask)] = ep->next;
    else {
        for (ep = first; ep->next != NULL; ep = ep->next) {
            if (ep->next->number == number) {
                epc = ep->next;
                ep->next = ep->next->next;
                break;
            }
        }
    }
    free(epc);
}

/* Destroys the given hash table.
hashTableSize is number of buckets. */
void freeHashTable(Element **hashTable, int hashTableSize) {
    Element *ep = NULL;                                         // pointer to an Element
    for (int i = 0; i < hashTableSize; i++) {
        if (hashTable[i]) {
            Element *epn = NULL;
            for (ep = hashTable[i]; ep->next != NULL; ep = epn) {
                epn = ep->next;
                free(ep);
            }
            free(ep);
        }
    }
}

/* As it name says, calculates and returns the nearest power of two of the input value.
This is needed for hashTableSize, which we want to be a power of two.
We could always pick either the first smaller or the first larger value,
but this solution pays attention to memory consumption.
Favours larger value slightly, in case they are equidistant from the input value.
Larger hash table means less possible collisions, and faster solution,
that uses more memory at the same time.
Also returns power, as an argument, which is == log2(out).
This is not really needed in hash() function, so it's left out for speed reasons,
but it's possible as a feature. */
unsigned int calculateNearestPowerOfTwo(int in /*, int *power */) {
    unsigned int out, inCopy = in;
    unsigned int smaller, larger;
    register unsigned int i;
    for (i = 0; inCopy > 0; i++) {
        inCopy >>= 1;
    }
    smaller = 1 << (i - 1);
    larger = 1 << i;
    out = in - smaller < larger - in ? smaller : larger;
    //*power = out == smaller ? i - 1 : i;
    return out;
}

void testCalculateNearestPowerOfTwo(void) {
    unsigned int power = 0;
    printf("%d ", calculateNearestPowerOfTwo(10000 /*, &power*/));
    printf("%d\n", power);
}


/* THE EXAMPLE USAGE CODE */

typedef struct Query Query;

struct Query {
    char type[5];
    int number;
    char name[MAX_NAME_LEN];
};

Query *readQueries(int *numQueries) {
    scanf("%d", numQueries);
    Query *queries = malloc(*numQueries * sizeof(*queries));
    for (int i = 0; i < *numQueries; i++) {
        scanf("%s", queries[i].type);
        scanf("%d", &(queries[i].number));
        if (!strcmp(queries[i].type, "add"))
            scanf("%s", queries[i].name);
    }
    return queries;
}

char **processQueries(Query *queries, int numQueries, int *resLen) {
    /* An array of pointers to strings. Used with memcpy() or strcpy(). */
    char **result = malloc(numQueries * sizeof(*result));
    /* A contiguous array of strings (2-D array of chars). Used with memcpy() or strcpy(). */
    result[0] = calloc(numQueries * MAX_NAME_LEN, sizeof(**result));
    for (int i = 1; i < numQueries; i++)
        result[i] = result[0] + i * MAX_NAME_LEN;

    const unsigned int numBuckets = (numQueries <= RATIO ? numQueries : numQueries / RATIO);
    /* Number of buckets, not elements. */
    const unsigned int contactsSize = calculateNearestPowerOfTwo(numBuckets);
    /* mask is used in hash() instead of hashTableSize. */
    const unsigned int mask = contactsSize - 1;
    /* Hash table: dynamic array of contactsSize pointers to Elements - has to be initialized to zeros (NULL pointers). */
    Element **contacts = calloc(contactsSize, sizeof(*contacts));

    for (int i = 0; i < numQueries; i++) {
        if (!(strcmp(queries[i].type, "add"))) {
            insert(contacts, mask, queries[i].number, queries[i].name);
        }
        else if (!(strcmp(queries[i].type, "del"))) {
            eraseDoubly(contacts, mask, queries[i].number);
        }
        else {                                                  // queries[i].type == "find"
            char *res = find(contacts, mask, queries[i].number);
            unsigned len = strlen(res);
            memcpy(result[(*resLen)++], res, len);              // We can pass MAX_NAME_LEN instead of len, which we don't have to calculate in that case.
        }
    }

    freeHashTable(contacts, contactsSize);
    free(contacts);
    free(queries);
    return result;
}

/* For use with memcpy() or strcpy() variants of processQueries(), which uses resLen and returns char** in that case. */
void writeResponses(char **result, int resLen) {
    for (int i = 0; i < resLen; i++) {
        printf("%s\n", result[i]);
    }
    free(result[0]);
    free(result);
}


int main(void) {
    int numQueries = 0;
    Query *queries = readQueries(&numQueries);
    int resLen = 0;
    char **result = processQueries(queries, numQueries, &resLen);
    writeResponses(result, resLen);

    char c = getchar();
    c = getchar();
    return 0;
}

/* Test data:

First row of input contains number of queries.
Possible commands are: add, find, del.

Input:
12
add 52368 Neo
add 46213 Mom
add 911 police
find 46213
find 912
find 911
del 912
del 911
find 911
find 46213
add 46213 smith
find 46213

Output:
Mom
not found
police
not found
Mom
smith

Input:
8
find 5558888
add 654321 me
add 0 johnny
find 0
find 654321
del 0
del 0
find 0

Output:
not found
johnny
me
not found
*/

#endif // PHONE_BOOK_ALT 