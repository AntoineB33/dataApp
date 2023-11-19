#ifndef MEDIA_SORTER_H
#define MEDIA_SORTER_H

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

typedef struct treeCons {
    int id;
    int afters;
    int mediaSize;
    int befSize;
    struct treeCons** before;
    int attrPSize;
    attrProp* attrP;
} treeCons;

typedef struct {
    int last;
    int dist;
    int rest;
    int prevLast;
    int prevRest;
} attribute;

// Function prototypes
void* sortTable(void* id);
int initSort(char *argv);
char* askFile();
void sha256(const char *str, unsigned char hash[EVP_MAX_MD_SIZE]);

#endif
