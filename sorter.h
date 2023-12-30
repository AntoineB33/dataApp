#ifndef INIT_SORT_H
#define INIT_SORT_H

#include <openssl/evp.h>

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

#define INITIAL_LINE_LENGTH 100
#define CHUNK_SIZE 256
#define INITIAL_LINE_NUMBER 150
#define INITIAL_ARG_NUMBER 50
#define CHUNK_ARG_NB 50 


typedef struct {
    int posInt;
    int posOut;
    int attr;
} attrProp;

typedef struct {
    int attr;
    int follower;
} folCat;

typedef struct {
    int afters;
    int mediaSize;
    int befSize;
    int* before;
    int attrPSize;
    attrProp* attrP;
    int folCatSize;
    folCat* folCat;
} treeCons;

typedef struct {
    int prevLast;
    int prevRest;
} prevStT;

typedef struct {
    int* rest[2];
    int size[2];
    int choice;
    int error;
    int loner;
    // int space;
} restStT;

typedef struct {
    int last;
    int dist;
    int rest;
    int prevSize0;
    int prevSize;
    prevStT* prevSt;
} attribute;

// Function prototypes
void* sortTable();
void initSort(char*);
void catFolUpdate(treeCons* trees, restStT* rest, treeCons* po2, int inc, int i);

#endif
