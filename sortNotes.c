#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <fcntl.h>
#include <sys/file.h>

#include <dirent.h>

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


int lenAgg;
int attNb;
treeCons* trees = NULL;
char** output = NULL;
char* txt = NULL;
attribute* attributes = NULL;
int error;
int space;
int loner;
int lvl;
pthread_rwlock_t checkM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t fileM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t errorM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t lonerM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t errorWM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t lonerWM = PTHREAD_RWLOCK_INITIALIZER;
pthread_t *threads = NULL;
int txtSize;
FILE* file;
char* filePATH = NULL;


void freeAll() {
    free(threads);
    free(attributes);
    free(txt);
    for(int i = 0; i<lenAgg; i++) {
        free(trees[i].attrP);
        free(trees[i].before);
    }
    free(trees);
    exit(0);
}

void sha256(const char *str, unsigned char hash[EVP_MAX_MD_SIZE]) {
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    OpenSSL_add_all_digests();

    md = EVP_get_digestbyname("sha256");

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str, strlen(str));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    for(i = 0; i < md_len; i++)
        sprintf(&hash[i*2], "%02x", md_value[i]);
}

void* sortTable(void* id) {
    intptr_t threadId = (intptr_t)id;
    int lenAggP = lenAgg+1;
    int* res = malloc(lenAggP * sizeof(int));
    int* spaces = malloc((lenAggP) * sizeof(int));
    int* loners = malloc((lenAggP) * sizeof(int));
    int* errori = malloc((lenAggP) * sizeof(int));
    spaces[0] = 0;
    loners[0] = 0;
    errori[0] = 0;
    int errorF; // copy of error
    int q;  // qth attribute
    attrProp* y;
    attribute* attributesI = malloc(attNb * sizeof(attribute));
    for(int j = 0; j<attNb; j++) {
        attributesI[j] = attributes[j];
    }
    int i;  // ith node to place
    int imm;    // i-1
    int n;  // nth medium
    int c;
    bool loop = true;
    int back = 0;
    treeCons* po;
    bool compar = false;
    int k;
    int xc;
    int* u;
    int* cal;
    int errorP;
    int lonerP;
    bool newSort = false;



    while(1) {
        i = 1;
        imm = i-1;
        n = 0;
        while (1) {
            while(i>lvl && i<lenAggP) {
                if(back==0) {
                    res[i] = 0;
                } else {
                    po = &trees[res[i]];
                    for(int k = 0; k<po->attrPSize; k++) {
                        q = po->attrP[k].attr;
                        attributesI[q].last = attributesI[q].prevLast;
                        attributesI[q].rest = attributesI[q].prevRest;
                    }
                    if(back==2) {
                        imm--;
                        n-=trees[res[i]].mediaSize;
                        trees[res[i]].afters++;
                        for(int k = 0; k<trees[res[i]].befSize; k++) {
                            trees[res[i]].before[k]->afters++;
                        }
                    }
                    res[i]++;
                }
                while(res[i]<lenAgg && trees[res[i]].afters) {
                    res[i]++;
                }
                if(res[i]==lenAgg) {
                    back = 2;
                    i--;
                    continue;
                }
                errori[i] = errori[imm];
                spaces[i] = spaces[imm];
                loners[i] = loners[imm];
                for(int k = 0; k<trees[res[i]].attrPSize; k++) {
                    y = &trees[res[i]].attrP[k];
                    q = y->attr;
                    attributesI[q].prevLast = attributesI[q].last;
                    attributesI[q].prevRest = attributesI[q].rest;
                    if(attributesI[q].last<0) {
                        if(attributesI[q].last==-1) {
                            errori[i] += n+y->posInt;
                            attributesI[q].last = n+y->posOut;
                        } else {
                            c = attributesI[q].dist-n;
                            if(c>0) {
                                loners[i] += c;
                            } else if(c<0) {
                                loners[i] -= c;
                                if(attributesI[q].rest>0) {
                                    loners[i] += c-1;
                                } else {
                                    loners[i] += c;
                                }
                            }
                        }
                    } else {
                        c = attributesI[q].dist-n+attributesI[q].last-y->posInt;
                        if(c>0) {
                            if(attributesI[q].rest>0) {
                                errori[i] += c-1;
                                attributesI[q].rest--;
                            } else {
                                errori[i] += c;
                            }
                        } else {
                            spaces[i]-=c;
                        }
                        attributesI[q].last = n+y->posOut;
                    }
                }
                pthread_rwlock_rdlock(&errorM);
                errorP = error;
                pthread_rwlock_unlock(&errorM);
                if(errori[i]>errorP) {
                    back = 1;
                    continue;
                }
                if(errori[i]==errorP) {
                    pthread_rwlock_rdlock(&lonerM);
                    lonerP = loner;
                    pthread_rwlock_unlock(&lonerM);
                    if(loners[i]>=lonerP) {
                        back = 1;
                        continue;
                    }
                }
                trees[res[i]].afters--;
                for(int k = 0; k<trees[res[i]].befSize; k++) {
                    trees[res[i]].before[k]->afters--;
                }
                n+=trees[res[i]].mediaSize;
                i++;
                imm++;
                back = 0;
            }
            if(i==lvl) {
                break;
            }
            i--;
            pthread_rwlock_rdlock(&checkM);
            pthread_rwlock_rdlock(&errorWM);
            if(errori[i]<error) {
                pthread_rwlock_wrlock(&errorM);
                error = errori[i];
                pthread_rwlock_unlock(&errorM);
                pthread_rwlock_wrlock(&lonerM);
                loner = loners[i];
                pthread_rwlock_unlock(&lonerM);
                pthread_rwlock_unlock(&errorWM);
                back = 2;
            } else {
                if (errori[i]==error) {
                    pthread_rwlock_unlock(&errorWM);
                    pthread_rwlock_rdlock(&lonerWM);
                    if(loners[i]<loner) {
                        pthread_rwlock_wrlock(&lonerM);
                        loner = loners[i];
                        pthread_rwlock_unlock(&lonerM);
                        pthread_rwlock_unlock(&lonerWM);
                        back = 1;
                    } else {
                        pthread_rwlock_unlock(&lonerWM);
                        back = 0;
                    }
                } else {
                    pthread_rwlock_unlock(&errorWM);
                }
            }
            if(back) {
                pthread_rwlock_rdlock(&fileM);
                pthread_rwlock_unlock(&checkM);
                txt[0] = '\0';
                for(int j = 1; j<lenAggP; j++) {
                    sprintf(txt, "%s%d,", txt, res[j]);
                }
                file = fopen(filePATH, "w");
                if (file == NULL) {
                    printf("Failed to open the output file.\n");
                    freeAll();
                }
                int fileNo = fileno(file);
                if (flock(fileNo, LOCK_EX) == -1) {
                    perror("Failed to obtain lock");
                    freeAll();
                }
                txt[strlen(txt)-1] = '\0';
                fprintf(file, "%s", txt);
                flock(fileNo, LOCK_UN);
                fclose(file);
                pthread_rwlock_unlock(&fileM);
            } else{
                pthread_rwlock_unlock(&checkM);
            }
            back = 2;
        }
        break;
    }
    free(res);
    free(spaces);
    free(loners);
    free(errori);
    free(attributesI);
}
int main(int argc, char *argv[]) {

    error = INT_MAX;
    space = 0;
    loner = INT_MAX;
    filePATH = malloc(strlen(argv[1]) + 10);
    sprintf(filePATH, "data/%s.txt", argv[1]);

    file = fopen(filePATH, "r");
    if (file == NULL) {
        printf("Failed to open the file %s\n", filePATH);
        lenAgg = 0;
        freeAll();
    }
    int filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        perror("Failed to obtain lock");
        lenAgg = 0;
        freeAll();
    }
    
    
    char* line;
    size_t len = 0;
    getline(&line, &len, file);
    if(line[0]=='"') {
        line++;
    }
    char* token = strtok(line, "\t");
    token = strtok(NULL, "\t");
    lenAgg = atoi(token);
    token = strtok(NULL, "\t");
    int lenVal = atoi(token);
    token = strtok(NULL, "\t");
    attNb = atoi(token);
    getline(&line, &len, file);
    attributes = malloc(attNb * sizeof(attribute));
    token = strtok(line, "\t");
    for(int i = 0; i<attNb; i++) {
        attributes[i].dist = atoi(token);
        token = strtok(NULL, "\t");
    }
    int count;
    trees = malloc(lenAgg * sizeof(treeCons));
    int** precRef = malloc(lenAgg * sizeof(int*));
    int mediaNb = 0;
    for(int i = 0; i<lenAgg; i++) {
        trees[i].id = i;
        getline(&line, &len, file);
        getline(&line, &len, file);
        token = strtok(line, ",");
        trees[i].afters = atoi(token);
        token = strtok(NULL, ",");
        trees[i].mediaSize = atoi(token);
        mediaNb += trees[i].mediaSize;

        getline(&line, &len, file);
        count = 0;
        precRef[i] = malloc(lenAgg * sizeof(int));
        token = strtok(line, ",");
        while(token!=NULL && token[0] != '\r') {
            precRef[i][count] = atoi(token);
            token = strtok(NULL, ",");
            count++;
        }
        trees[i].befSize = count;

        getline(&line, &len, file);
        trees[i].attrP = malloc(attNb * sizeof(attrProp));
        count = 0;
        token = strtok(line, ",");
        while(token!=NULL && token[0] != '\r') {
            trees[i].attrP[count].attr = atoi(token);
            token = strtok(NULL, ",");
            trees[i].attrP[count].posInt = atoi(token);
            token = strtok(NULL, "\t");
            trees[i].attrP[count].posOut = atoi(token);
            token = strtok(NULL, ",");
            count++;
        }
        trees[i].attrPSize = count;
        trees[i].attrP = realloc(trees[i].attrP, count * sizeof(attrProp));
    }
    flock(filNo, LOCK_UN);
    fclose(file);
    for(int i = 0; i<lenAgg; i++) {
        trees[i].before = malloc(trees[i].befSize * sizeof(treeCons*));
        for(int j = 0; j<trees[i].befSize; j++) {
            trees[i].before[j] = &trees[precRef[i][j]];
        }
    }
    for(int i = 0; i<lenAgg; i++) {
        free(precRef[i]);
    }
    free(precRef);
    for(int i = 0; i<attNb; i++) {
        if(attributes[i].dist <= 0) {
            attributes[i].last = -2;
            attributes[i].dist = (mediaNb-1)/2-attributes[i].dist;
            attributes[i].rest = (mediaNb-1)%2;
        } else {
            attributes[i].last = -1;
            attributes[i].dist = mediaNb/(attributes[i].dist-1);
            attributes[i].rest = mediaNb%(attributes[i].dist-1);
        }
        attributes[i].prevLast = attributes[i].last;
        attributes[i].prevRest = attributes[i].rest;
    }

    int numCores = sysconf(_SC_NPROCESSORS_ONLN);
    threads = malloc(numCores * sizeof(pthread_t));
    lvl = 0;
    //txtSize = lenAgg * (2+log10(lenAgg));
    txtSize = 1000;
    filePATH = realloc(filePATH, strlen(filePATH) + 7);
    sprintf(filePATH, "data/%s_sorted.txt", argv[1]);
    txt = malloc(txtSize*lenAgg);
    sortTable(0);
    for(int i = 10; i<numCores; i++) {
        intptr_t arg = (intptr_t)i;
        if(pthread_create(&threads[i], NULL, sortTable, (void*)arg)!=0) {
            fprintf(stderr, "Error creating thread %d.\n", i);
            freeAll();
        }
    }
    for(int i = 10; i<numCores; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            freeAll();
        }
    }
    freeAll();
}