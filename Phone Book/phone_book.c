#define PHONE_BOOK
#ifdef PHONE_BOOK

/* Phone book */

/* RATIO is the ratio between numQueries (number of inputs) and number of buckets.
RATIO == 1 is faster than RATIO == 10.
If number of buckets is 1, meaning RATIO == numQueries, it works too slowly. */

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
#define HASH_TABLE_SIZE 10000u                                  // unused
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

/* Hash function for integers. */
unsigned int hash(int x, int hashTableSize) {
    return (unsigned int)(((((unsigned int)x << 5) + 1) % PRIME) % hashTableSize);
}

/* hashTableSize is number of buckets.
Private function. Used in insert(). */
Element *_find(Element **hashTable, int hashTableSize, int number) {
    Element *ep = NULL;                                         // pointer to Element
    for (ep = hashTable[hash(number, hashTableSize)]; ep != NULL; ep = ep->next) {
        if (ep->number == number)
            return ep;                                          // found
    }
    return NULL;                                                // not found
}

/* hashTableSize is number of buckets.
Public function.
It would be more general to return Element* (ep or NULL),
but it's faster to return a string, because in this case,
we don't have to check whether the pointer is NULL or not
in the calling routine. */
char *find(Element **hashTable, int hashTableSize, int number) {
    Element *ep = NULL;                                         // pointer to Element
    for (ep = hashTable[hash(number, hashTableSize)]; ep != NULL; ep = ep->next) {
        if (ep->number == number)
            return ep->name;                                    // found
    }
    return "not found";                                         // not found
}

/* Inserts an element if there's no element with the given number.
If there is the given number already, rewrites the element's name field.
hashTableSize is number of buckets.
Returns nothing. */
void insert(Element **hashTable, int hashTableSize, int number, char *name) {
    Element *ep = NULL;                                         // pointer to Element
    unsigned int hashValue = 0;
    if (!(ep = _find(hashTable, hashTableSize, number))) {      // not found
        ep = malloc(sizeof(*ep));                               // sizeof(Element)
        if (!ep)                                                // if malloc fails
            exit(-1);
        hashValue = hash(number, hashTableSize);
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
void eraseDoubly(Element **hashTable, int hashTableSize, int number) {
    Element *ep = NULL;
    if (!(ep = _find(hashTable, hashTableSize, number)))
        return;                                                 // not found
    if (!(ep->prev))
        hashTable[hash(number, hashTableSize)] = ep->next;
    else
        ep->prev->next = ep->next;
    if (ep->next)
        ep->next->prev = ep->prev;
    free(ep);
}

/* Erases the element with the given number, if it exists.
If it doesn't exist, ignores the request. */
void erase(Element **hashTable, int hashTableSize, int number) {
    Element *ep = NULL;
    if (!(ep = _find(hashTable, hashTableSize, number)))
        return;                                                 // not found
    Element *first = hashTable[hash(number, hashTableSize)];
    Element *epc = ep;
    if (first == ep)
        hashTable[hash(number, hashTableSize)] = ep->next;
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
    Element *ep = NULL;                                         // pointer to Element
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


/* THE EXAMPLE USAGE CODE */

typedef struct Query Query;

struct Query {
    char type[5];
    int number;
    char name[MAX_NAME_LEN];
};

/* Slow. */
/* Reads queries from stdin and returns them.
It also returns total number of queries and number of elements,
via parameters list.
numElements can be used not only to save memory when allocating it for
the hash table "contacts", but it might also mean a difference between
working with 32-bit or 64-bit values, in alternative alnernative solution.
But that's not very likely, because that phone book would be really large,
unrealistically large in fact.
Besides, this function only works when we know all queries in advance;
but, that's not the case in reality, where we
would have to have a larger hash table.
Actually, it would work, but maybe slower, because hash table might
be too small, and load factor too large. */
Query *readQueriesThrifty(int *numQueries, int *numElements) {
    scanf("%d", numQueries);
    Query *queries = malloc(*numQueries * sizeof(*queries));
    *numElements = 0;
    for (int i = 0; i < *numQueries; i++) {
        scanf("%s", queries[i].type);
        scanf("%d", &(queries[i].number));
        if (!strcmp(queries[i].type, "add")) {
            scanf("%s", queries[i].name);
            (*numElements)++;
        }
        else if (!strcmp(queries[i].type, "del") && (*numElements > 0)) {
            (*numElements)--;
        }
    }
    return queries;
}

/* Reads queries from stdin and returns them.
It also returns total number of queries and number of elements set to 0,
via parameters list. numElements is 0 because we don't use it here;
this is a general readQueries() function. */
Query *readQueries(int *numQueries, int *numElements) {
    *numElements = 0;
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

char **processQueries(Query *queries, int numQueries, int *resLen, int numElements) {
    /* A single string to hold the result. Used with strcat(). */
    //char *result = calloc(numQueries, MAX_NAME_LEN);

    /* An array of pointers to strings. Used with memcpy() or strcpy(). */
    char **result = malloc(numQueries * sizeof(*result));
    /* A contiguous array of strings (2-D array of chars). Used with memcpy() or strcpy(). */
    result[0] = calloc(numQueries * MAX_NAME_LEN, sizeof(**result));
    for (int i = 1; i < numQueries; i++)
        result[i] = result[0] + i * MAX_NAME_LEN;

    /* If we work with readQueries(), it will always return numElements == 0,
    so we should set it to numQueries.
    On the other hand, if we work with readQueriesThrifty(), that means
    we want the exact numElements as returned by that function. */
    numElements = numElements ? numElements : numQueries;
    /* Number of buckets, not elements. */
    const int contactsSize = (numElements <= RATIO ? numElements : numElements / RATIO);
    /* Hash table: dynamic array of contactsSize pointers to Elements - has to be initialized to zeros (NULL pointers). */
    Element **contacts = calloc(contactsSize, sizeof(*contacts));

    for (int i = 0; i < numQueries; i++) {
        if (!(strcmp(queries[i].type, "add"))) {
            insert(contacts, contactsSize, queries[i].number, queries[i].name);
        }
        else if (!(strcmp(queries[i].type, "del"))) {
            eraseDoubly(contacts, contactsSize, queries[i].number);
        }
        else {                                                  // queries[i].type == "find"
            /* Fast. */
            //printf("%s\n", find(contacts, contactsSize, queries[i].number));

            /* Slow. */
            //strcat(result, find(contacts, contactsSize, queries[i].number));
            //strcat(result, "\n");

            /* Fast. */
            //strcpy(result[(*resLen)++], find(contacts, contactsSize, queries[i].number));
            
            /* Fast. */
            char *res = find(contacts, contactsSize, queries[i].number);
            unsigned len = strlen(res);
            memcpy(result[(*resLen)++], res, len);              // We can pass MAX_NAME_LEN instead of len, which we don't have to calculate in that case.
            //memcpy(result[(*resLen) - 1] + len, "\n", 1);       // Requires MAX_NAME_LEN == 17, because of "\n", but works in that case, with same speed as usual. Also, we shouldn't print "\n" in writeResponses().
        }
    }

    freeHashTable(contacts, contactsSize);
    free(contacts);
    free(queries);
    return result;
}

/* Slow. */
/* For use with strcat() variant of processQueries(), which returns char* in that case. */
void writeResponsesSingleString(char *result) {
    printf("%s", result);
    free(result);
}

/* Fast. */
/* For use with memcpy() or strcpy() variants of processQueries(), which uses resLen and returns char** in that case. */
void writeResponses(char **result, int resLen) {
    for (int i = 0; i < resLen; i++) {
        printf("%s\n", result[i]);
    }
    free(result[0]);
    free(result);
}

int main(void) {
    int numQueries = 0, numElements = 0;
    Query *queries = readQueries(&numQueries, &numElements);
    int resLen = 0;
    char **result = processQueries(queries, numQueries, &resLen, numElements);
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

#endif // PHONE_BOOK 
