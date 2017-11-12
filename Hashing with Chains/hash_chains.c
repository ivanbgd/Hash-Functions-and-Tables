#define HASH_CHAINS
#ifdef HASH_CHAINS

/* Hashing with chains */

/* Works on strings. */

/* This variant works with global variables, in contrast to "Phone Book",
which passes number of buckets to appropriate functions.
This makes it faster, beside making it easier to code.
Of course, this is a single compilation unit project, so there can be no
problem with using global variables.

In this example, hash function expects particular values of
PRIME and X (the multiplier) to work properly. If we change either PRIME or X,
this example won't work as expected, even with the same code for hash function. */


#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_ITEMS 100000
#define TRUE 1
#define FALSE 0
#define PRIME 1000000007u                                       // This value must not be changed.
#define X 263                                                   // Multiplier; this value must not be changed.
#define MAX_STRING_LEN 16                                       // 15 + 1 for the terminating character

/* HASH TABLE CODE */

/* Hash table size (number of buckets) */
size_t numBuckets;

typedef struct Element Element;

struct Element {
    char s[MAX_STRING_LEN];
    Element *prev, *next;
};

/* Hash function for strings. */
size_t hash(char *s) {
    unsigned long long h = 0;
    size_t slen = strlen(s);

    for (register int i = slen - 1; i >= 0; --i)
        h = (h * X + s[i]) % PRIME;

    //printf("%llu", h % numBuckets);
    return h % numBuckets;
}

/* Private function. Used in insert() and erase(). */
Element *_find(Element **hashTable, char *s) {
    /* Pointer to Element. */
    Element *ep = NULL;
    for (ep = hashTable[hash(s)]; ep != NULL; ep = ep->next) {
        if (!strcmp(ep->s, s))
            return ep;                                          // found
    }
    return NULL;                                                // not found
}

/* Public function.
It would be more general to return Element* (ep or NULL),
but it's faster to print a string, because in this case,
we don't have to check whether the pointer is NULL or not
in the calling routine, and we don't have to copy the
string - we can just print it here. */
void find(Element **hashTable, char *s) {
    /* Pointer to Element. */
    Element *ep = NULL;
    for (ep = hashTable[hash(s)]; ep != NULL; ep = ep->next) {
        if (!strcmp(ep->s, s)) {
            printf("yes\n");                                    // found
            return;
        }
    }
    printf("no\n");                                             // not found
    return;
}

/* Inserts an element if there's no element with the given string.
If there is the given string already, does nothing (ignores the request).
Returns nothing. */
void insert(Element **hashTable, char *s) {
    /* Pointer to Element. */
    Element *ep = NULL;
    if (!(ep = _find(hashTable, s))) {                          // not found
        ep = malloc(sizeof(*ep));                               // sizeof(Element)
        if (!ep)                                                // if malloc fails
            exit(-1);
        size_t hashValue = hash(s);
        strcpy(ep->s, s);
        ep->prev = NULL;                                        // this element will be the first one in the bucket
        ep->next = hashTable[hashValue];                        // always references the first element of the bucket (even if it's NULL)
        if (ep->next)
            ep->next->prev = ep;
        hashTable[hashValue] = ep;                              // adds this pointer as the first one in the bucket
    }
}

/* Erases the element with the given string, if it exists.
If it doesn't exist, ignores the request. */
void erase(Element **hashTable, char *s) {
    /* Pointer to Element. */
    Element *ep = NULL;
    if (!(ep = _find(hashTable, s)))
        return;                                                 // not found
    if (!(ep->prev))
        hashTable[(hash(s))] = ep->next;
    else
        ep->prev->next = ep->next;
    if (ep->next)
        ep->next->prev = ep->prev;
    free(ep);
}

/* Destroys the given hash table. */
void freeHashTable(Element **hashTable) {
    /* Pointer to Element. */
    Element *ep = NULL;
    for (size_t i = 0; i < numBuckets; i++) {
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


/* THE EXAMPLE USAGE CODE */

typedef struct Query Query;

struct Query {
    char type[6], s[MAX_STRING_LEN];
    size_t ind;
};

Query *readQuery(void) {
    static Query query;
    scanf("%s", query.type);
    if (strcmp(query.type, "check"))
        /* add, del, find */
        scanf("%s", query.s);
    else
        /* check */
        scanf("%u", &(query.ind));
    return &query;
}

void processQuery(Query *query, Element **contacts) {
    if (!strcmp(query->type, "check")) {
        /* ep will first point to the first element in a bucket; or it will stay NULL. */
        Element *ep = NULL;
        /* This for loop traverses a bucket; from the first element to the last one. */
        for (ep = contacts[query->ind]; ep != NULL; ep = ep->next)
            printf("%s ", ep->s);
        printf("\n");
    }
    else {
        if (!strcmp(query->type, "find"))
            find(contacts, query->s);
        else if (!strcmp(query->type, "add"))
            insert(contacts, query->s);
        else
            /* query.type == "del" */
            erase(contacts, query->s);
    }
}

void processQueries(void) {
    /* Hash table: dynamic array of numBuckets pointers to Elements - has to be initialized to zeros (NULL pointers). */
    Element **contacts = calloc(numBuckets, sizeof(*contacts));

    size_t numQueries;
    scanf("%u", &numQueries);

    for (size_t i = 0; i < numQueries; ++i)
        processQuery(readQuery(), contacts);

    freeHashTable(contacts);
    free(contacts);
}


int main(void) {
    scanf("%u", &numBuckets);
    processQueries();

    char c = getchar();
    c = getchar();
    return 0;
}

/* Test data:

The first row of input contains number of buckets.
The second row of input contains number of queries.
Possible commands are: add, find, del, check.
Output is mixed with input.

Input:
5
12
add world
add HellO
check 4
find World
find world
del world
check 4
del HellO
add luck
add GooD
check 2
del good

Output:
HellO world
no
yes
HellO
GooD luck

Input:
4
8
add test
add test
find test
del test
find test
find Test
add Test
find Test

Output:
yes
no
no
yes

Input:
3
12
check 0
find help
add help
add del
add add
find add
find del
del del
find del
check 0
check 1
check 2

Output:
no
yes
yes
no
add help
*/

#endif // HASH_CHAINS 
