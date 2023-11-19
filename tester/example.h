#ifndef EXAMPLE_H
#define EXAMPLE_H

// Define the structure in the header file
typedef struct {
    int id;
    char name[50];
    float salary;
} Employee;


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
void freeTrees(treeCons* var, int len);
void freeList(char **var, int len);
void freeVar(void *var);
void* sortTable(void* id);
int initSort(char *argv);
char* askFile();

// Function prototype using the structure
void printEmployeeInfo(Employee emp);

#endif /* EXAMPLE_H */
