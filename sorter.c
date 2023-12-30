#include "sorter.h"
#include "fileLocker.h"
#include "readLine.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>
#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>


int lenAgg;
int attNb;
treeCons* trees0;
char** output = NULL;
char* txt2 = NULL;
attribute* attributes = NULL;
int error;
int space;
int loner;
int errorP;
int lvl;
pthread_rwlock_t newRoot = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t checkM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t fileM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t errorM = PTHREAD_RWLOCK_INITIALIZER;
int errorW = false;
pthread_rwlock_t errorWM = PTHREAD_RWLOCK_INITIALIZER;
pthread_t *threads = NULL;
FILE* file = NULL;
char* filePATH = NULL;
size_t len;
int lastThRoot;
char* temp = NULL;
HANDLE hFile;
int numCores;
bool waitsInput = true;
HANDLE mutex;


void loop(int head, treeCons* trees, restStT* rest, attribute* attributesI, int* i, int* n, int back, int* lastThRootP, int* nbLeaves) {
    int nbLeavesi = 0;
    int imm = (*i)-1;
    attrProp* y;
    attribute* attrP;
    int c;
    int ri;
    treeCons* po2;
    prevStT* prevIP;
    int* choice;
    while(1) {
        while((!head && *i>lvl && *i<=lenAgg) || (head && *i>0 && *i<=lvl)) {
            choice = &rest[*i].choice;
            if(back==0) {
                *choice = 0;
            } else {
                po2 = &trees[rest[*i].rest[0][*choice]];
                for(int k = 0; k<po2->attrPSize; k++) {
                    attrP = &attributesI[po2->attrP[k].attr];
                    if(attrP->last!=-2) {
                        prevIP = &attrP->prevSt[--attrP->prevSize];
                        attrP->last = prevIP->prevLast;
                        attrP->rest = prevIP->prevRest;
                    }
                }
                if(back==2) {
                    imm--;
                    *n-=po2->mediaSize;
                    catFolUpdate(trees, rest, po2, -1, *i);
                }
                (*choice)++;
            }
            if(*choice==rest[*i].size[0]) {
                back = 2;
                (*i)--;
                continue;
            }
            rest[*i].error = rest[imm].error;
            rest[*i].loner = rest[imm].loner;
            po2 = &trees[rest[*i].rest[0][*choice]];

            for(int k = 0; k<(po2->attrPSize); k++) {
                y = &po2->attrP[k];
                attrP = &attributesI[y->attr];
                if(attrP->last==-2) {
                    c = attrP->dist-*n;
                    if(c>0) {
                        rest[*i].loner += c;
                    } else if(c<0) {
                        rest[*i].loner -= c;
                        if(attrP->rest>0) {
                            rest[*i].loner += c-1;
                        } else {
                            rest[*i].loner += c;
                        }
                    }
                } else {
                    prevIP = &attrP->prevSt[attrP->prevSize++];
                    prevIP->prevLast = attrP->last;
                    prevIP->prevRest = attrP->rest;
                    if(attrP->last==-1) {
                        rest[*i].error += *n+y->posInt;
                        attrP->last = *n+y->posOut-1;
                    } else {
                        c = attrP->dist-*n+attrP->last-y->posInt;
                        if(c>0) {
                            if(attrP->rest>0) {
                                rest[*i].error += c-1;
                                attrP->rest--;
                            } else {
                                rest[*i].error += c;
                            }
                        }
                        attrP->last = *n+y->posOut-1;
                    }
                }
            }
            pthread_rwlock_rdlock(&errorM);
            errorW = true;
            if(rest[*i].error>error) {
                back = 1;
                pthread_rwlock_unlock(&errorM);
                errorW = false;
                continue;
            }
            if(rest[*i].error==error) {
                if(rest[*i].loner>=loner) {
                    back = 1;
                    pthread_rwlock_unlock(&errorM);
                    errorW = false;
                    continue;
                }
            }
            pthread_rwlock_unlock(&errorM);
            errorW = false;
            catFolUpdate(trees, rest, po2, 1, *i);
            rest[*i+1].size[0] = 0;
            rest[*i+1].size[1] = 0;
            for(int rk = 0; rk<2; rk++) {
                for(int k = 0; k<rest[*i].size[rk]; k++) {
                    if(rk==1 || k!=*choice) {
                        ri = trees[*choice].afters!=0;
                        rest[*i+1].rest[ri][rest[*i+1].size[ri]] = *i;
                        rest[*i+1].size[ri]++;
                    }
                }
            }
            *n+=po2->mediaSize;
            (*i)++;
            imm++;
            back = 0;
        }
        if(!head) {
            if(*i==lvl) {
                break;
            }
            (*i)--;
            pthread_rwlock_wrlock(&checkM);
            pthread_rwlock_rdlock(&errorWM);
            if(rest[*i].error<error) {
                pthread_rwlock_wrlock(&errorM);
                errorW = true;
                error = rest[*i].error;
                loner = rest[*i].error;
                pthread_rwlock_unlock(&errorM);
                errorW = false;
                back = 1;
            } else {
                if (rest[*i].error==error) {
                    if(rest[*i].loner<loner) {
                        pthread_rwlock_wrlock(&errorM);
                        errorW = true;
                        loner = rest[*i].loner;
                        pthread_rwlock_unlock(&errorM);
                        errorW = false;
                        back = 1;
                    } else {
                        back = 0;
                    }
                }
            }
            pthread_rwlock_unlock(&errorWM);
            if(back) {
                pthread_rwlock_wrlock(&fileM);
                pthread_rwlock_unlock(&checkM);
                txt2[0] = '\0';
                for(int j = 1; j<lenAgg+1; j++) {
                    sprintf(temp, "%d,", rest[j].rest[0][rest[j].choice]);
                    strcat(txt2, temp);
                }
                hFile = lock_and_open_file(filePATH);
                write_to_file(hFile, txt2);
                unlock_file(hFile);
                pthread_rwlock_unlock(&fileM);
            } else{
                pthread_rwlock_unlock(&checkM);
            }
            back = 2;
        } else if (head==1) {
            if(*i==0) {
                pthread_rwlock_unlock(&newRoot);
                return;
            }
            (*lastThRootP)++;
            if(*lastThRootP>lastThRoot) {
                break;
            }
            (*i)--;
            back = 2;
        } else {
            if(*i==0) {
                if(lenAgg==lvl) {
                    numCores = nbLeavesi;
                    if(lenAgg==0) {
                        numCores = 1;
                    }
                    break;
                }
                nbLeavesi = *nbLeaves;
                *nbLeaves = 0;
                lvl++;
            }
            (*nbLeaves)++;
            if(*nbLeaves==numCores) {
                break;
            }
        }
    }
}

void* sortTable() {
    treeCons* trees = malloc(lenAgg * sizeof(treeCons));
    restStT* rest = malloc((lenAgg+1) * sizeof(restStT));
    for(int i = 0; i<lenAgg; i++) {
        for(int ri = 0; ri<2; ri++) {
            rest[i+1].rest[ri] = malloc((lenAgg-i)*sizeof(int));
            rest[i+1].size[ri] = 0;
        }
        int ri = trees0[i].afters!=0;
        rest[1].rest[ri][rest[1].size[ri]] = i;
        rest[1].size[ri]++;
        trees[i].afters = trees0[i].afters;
        trees[i].mediaSize = trees0[i].mediaSize;
        trees[i].befSize = trees0[i].befSize;
        trees[i].before = malloc(trees[i].befSize * sizeof(int));
        for(int j = 0; j<trees[i].befSize; j++) {
            trees[i].before[j] = trees0[i].before[j];
        }
        trees[i].attrPSize = trees0[i].attrPSize;
        trees[i].attrP = malloc(trees[i].attrPSize * sizeof(attrProp));
        for(int j = 0; j<trees[i].attrPSize; j++) {
            trees[i].attrP[j] = trees0[i].attrP[j];
        }
        trees[i].folCatSize = trees0[i].folCatSize;
        trees[i].folCat = malloc(trees[i].folCatSize * sizeof(folCat));
        for(int j = 0; j<trees[i].folCatSize; j++) {
            trees[i].folCat[j] = trees0[i].folCat[j];
        }
    }
    for(int ri = 0; ri<2; ri++) {
        rest[1].rest[ri] = realloc(rest[1].rest[ri], rest[1].size[ri]*sizeof(int));
    }
    rest[0].loner = 0;
    rest[0].error = 0;
    attribute* attributesI = malloc(attNb * sizeof(attribute));
    for(int j = 0; j<attNb; j++) {
        attributesI[j].last = attributes[j].last;
        attributesI[j].dist = attributes[j].dist;
        attributesI[j].rest = attributes[j].rest;
        attributesI[j].prevSt = malloc(attributes[j].prevSize * sizeof(prevStT));
        attributesI[j].prevSize0 = attributes[j].prevSize;
        attributesI[j].prevSize = 0;
    }
    int i = 1;  // ith node to place
    int n = 0;  // nth medium
    int back = 0;
    int nbLeaves = 0;
    int lastThRootP = -1;
    while(1) {
        pthread_rwlock_wrlock(&newRoot);
        loop(1, trees, rest, attributesI, &i, &n, back, &lastThRootP, &nbLeaves);
        if(i==0) {
            break;
        }
        lastThRoot = lastThRootP;
        pthread_rwlock_unlock(&newRoot);
        loop(0, trees, rest, attributesI, &i, &n, back, &lastThRootP, &nbLeaves);
    }
    for(int j = 0; j<attNb; j++) {
        free(attributesI[j].prevSt);
    }
    free(attributesI);
    for(int j = 0; j<lenAgg; j++) {
        free(trees[j].attrP);
        free(trees[j].before);
        free(trees[j].folCat);
    }
    free(trees);
    return NULL;
}

void *userInterr() {
    printf("Press q to stop the sorting.\n");
    int ch;
    while (waitsInput) {
        WaitForSingleObject(mutex, INFINITE);

        // Check if the condition is satisfied by other threads

        // Check for user input without blocking
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'q') {
                ReleaseMutex(mutex);
                printf("\nUser pressed 'q'. Exiting...\n");
                pthread_rwlock_wrlock(&errorM);
                errorW = 1;
                errorP = error;
                error = -1;
                pthread_rwlock_unlock(&errorM);
                errorW = false;
                break;
            }
        }

        ReleaseMutex(mutex);
        Sleep(100); // Sleep for a short duration to avoid high CPU usage
    }
    if (waitsInput) {
        waitsInput = false;
    }
    pthread_exit(NULL);
    return NULL;
}

void catFolUpdate(treeCons* trees, restStT* rest, treeCons* po2, int inc, int i) {
    treeCons* po;
    for(int k = 0; k<po2->folCatSize; k++) {
        for(int ri = 0; ri<2; ri++) {
            if(po2->folCat[k].follower==-1) {
                for(int h = 0; h<rest[i].size[ri]; h++) {
                    po = &trees[rest[i].rest[ri][h]];
                    for(int bre = 0; bre<po->attrPSize; bre++) {
                        if(po->attrP[bre].attr==po2->folCat[k].attr) {
                            po->afters-=inc;
                            break;
                        }
                    }
                }
            } else {
                for(int h = 0; h<rest[i].size[ri]; h++) {
                    po = &trees[rest[i].rest[ri][h]];
                    for(int bre = 0; bre<po->attrPSize; bre++) {
                        if(po->attrP[bre].attr==po2->folCat[k].attr) {
                            po->afters+=inc;
                            break;
                        }
                    }
                }
            }
        }
    }
    for(int k = 0; k<po2->befSize; k++) {
        trees[po2->before[k]].afters-=inc;
    }
    (void)i;
    (void)rest;
}

void initSort(char *argv) {


    filePATH = malloc(strlen(argv) + 10);
    sprintf(filePATH, "data/%s.txt", argv);
    lenAgg = 0;

    // Open the file in binary read mode
    hFile = lock_and_open_file(filePATH);
    char* file_contents = readFileContentsFromHandle(hFile);
    unlock_file(hFile);


    char* backupPath = malloc(strlen(argv) + 20);
    sprintf(backupPath, "data/%s_backup.txt", argv);
    // Open the file in binary read mode
    hFile = lock_and_open_file(backupPath);
    char *file_backup = readLineFromHandle(hFile);
    char* token = strtok(file_backup, "\t");
    token = strtok(NULL, "\t");
    error = atoi(token);
    token = strtok(NULL, "\n");
    loner = atoi(token);
    space = 0;
    free(file_backup);
    file_backup = readFileContentsFromHandle(hFile);
    unlock_file(hFile);
    if(strcmp(file_contents,file_backup)!=0) {
        error = INT_MAX;
        space = 0;
        loner = INT_MAX;
    }
    free(file_backup);



    char* file_contents2 = malloc(strlen(file_contents) + 1);
    strcpy(file_contents2,file_contents);
    token = strtok(file_contents, "\t");
    token = strtok(NULL, "\t");
    lenAgg = atoi(token);
    token = strtok(NULL, "\t");
    int lenVal = atoi(token);
    token = strtok(NULL, "\t");
    attNb = atoi(token);


    attributes = malloc(attNb * sizeof(attribute));
    int matrixSize = lenAgg*4+lenVal+2;
    char** matrix = malloc(matrixSize * sizeof(char*));
    token = strtok(NULL, "\n");
    for(int i = 0; i<matrixSize; i++) {
        token = strtok(NULL, "\n");
        matrix[i] = malloc(strlen(token) + 1);
        strcpy(matrix[i],token);
    }
    free(file_contents);
    token = strtok(matrix[0], ",");
    for(int i = 0; i<attNb; i++) {
        attributes[i].prevSize = atoi(token);
        token = strtok(NULL, ",");
    }


    int count;
    int count2;
    trees0 = malloc(lenAgg * sizeof(treeCons));
    int mediaNb = 0;
    char** catFolMat = malloc(sizeof(char*));
    for(int i = 0; i<lenAgg; i++) {
        token = strtok(matrix[i*4+1], "\t");
        token = strtok(NULL, ",;\r");
        count = 0;
        while(token!=NULL) {
            count++;
            catFolMat = realloc(catFolMat, count * sizeof(char*));
            catFolMat[count-1] = malloc(strlen(token) + 1);
            strcpy(catFolMat[count-1],token);
            token = strtok(NULL, ",;\r");
        }
        trees0[i].folCatSize = count/2;
        trees0[i].folCat = malloc(count/2 * sizeof(folCat));
        for(int j = 0; j<count/2; j++) {
            trees0[i].folCat[j].attr = atoi(catFolMat[2*j]);
            trees0[i].folCat[j].follower = atoi(catFolMat[2*j+1]);
        }
        token = strtok(matrix[i*4+2], ",");
        trees0[i].afters = atoi(token);
        token = strtok(NULL, "\t");
        trees0[i].mediaSize = atoi(token);

        token = strtok(NULL, ";\r");
        count2 = 0;
        while(token!=NULL) {
            count2++;
            catFolMat = realloc(catFolMat, count2 * sizeof(char*));
            catFolMat[count2-1] = malloc(strlen(token) + 1);
            strcpy(catFolMat[count2-1],token);
            token = strtok(NULL, ";\r");
        }
        trees0[i].folCatSize = count2;
        trees0[i].folCat = realloc(trees0[i].folCat,(count2+count/2) * sizeof(folCat));
        for(int j = 0; j<count2; j++) {
            trees0[i].folCat[count+j].attr = atoi(catFolMat[j]);
            trees0[i].folCat[count+j].follower = -1;
        }


        mediaNb += trees0[i].mediaSize;
        count = 0;
        trees0[i].before = malloc(lenAgg * sizeof(int));
        token = strtok(matrix[i*4+3], ",");
        while(token!=NULL && token[0]!='\r') {
            trees0[i].before[count] = atoi(token);
            count++;
            token = strtok(NULL, ",");
        }
        trees0[i].befSize = count;

        trees0[i].attrP = malloc(attNb * sizeof(attrProp));
        count = 0;
        token = strtok(matrix[i*4+4], ",");
        while(token!=NULL && token[0]!='\r') {
            trees0[i].attrP[count].attr = atoi(token);
            token = strtok(NULL, ",");
            trees0[i].attrP[count].posInt = atoi(token);
            token = strtok(NULL, "\t");
            trees0[i].attrP[count].posOut = atoi(token);
            token = strtok(NULL, ",");
            count++;
        }
        trees0[i].attrPSize = count;
        trees0[i].attrP = realloc(trees0[i].attrP, count * sizeof(attrProp));
    }
    for(int i = 0; i<matrixSize; i++) {
        free(matrix[i]);
    }
    free(matrix);
    for(int i = 0; i<attNb; i++) {
        if(attributes[i].prevSize <= 0) {
            attributes[i].last = -2;
            attributes[i].dist = (mediaNb-1)/2-attributes[i].prevSize; // here dist is the opposite of the distance to the barycenter in the aggreg
            attributes[i].rest = (mediaNb-1)%2;
        } else {
            attributes[i].last = -1;
            attributes[i].rest = (mediaNb-1)%(attributes[i].prevSize-1);
            attributes[i].dist = (mediaNb-1)/(attributes[i].prevSize-1);
        }
    }
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    numCores = sysInfo.dwNumberOfProcessors;
    numCores = 1;
    threads = malloc((numCores+1) * sizeof(pthread_t));


    
    
    
    lastThRoot = -1;

    int txtSize = 1;
    int length = 2;  // Length of the current digit range (starting from 1 digit)
    int multiplier = 9;  // Multiplier for the number of digits in the current range
    int n = lenAgg;
    while (n > 0) {
        // Calculate the number of digits contributed by the current digit range
        txtSize += (n < multiplier) ? n * length : multiplier * length;
        n -= multiplier;
        length++;
        multiplier *= 10;  // Increase the multiplier for the next digit range
    }
    txt2 = malloc(txtSize);
    txt2[0] = '\0';
    n = lenAgg;
    length = 0;
    while (n != 0) {
        length++;
        n /= 10;
    }
    temp = malloc(length+2);
    // error = -1;
    filePATH = realloc(filePATH, strlen(filePATH) + 10);
    sprintf(filePATH, "data/%s_sorted.txt", argv);
    free(argv);
    lvl = 1;
    for(int i = 0; i<numCores; i++) {
        if(pthread_create(&threads[i], NULL, sortTable, NULL)!=0) {
            printf("Error creating thread %d.\n",i);
            exit(EXIT_FAILURE);
        }
    }
    if(pthread_create(&threads[numCores], NULL, userInterr, NULL)!=0) {
        printf("Error creating thread userInterr.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i<numCores; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    waitsInput = false;
    if (pthread_join(threads[numCores], NULL) != 0) {
        printf("pthread_join");
        exit(EXIT_FAILURE);
    }
    int ns[2] = {errorP, loner};
    int newSize = 0;
    for(int i = 0; i<2; i++) {
        n = ns[i];
        length = 0;
        while (n != 0) {
            length++;
            n /= 10;
        }
        newSize += length;
    }
    char* new_content = malloc(strlen(file_contents2)+newSize+3);
    sprintf(new_content, "%d\t%d\n%s", errorP, loner, file_contents2);
    hFile = lock_and_open_file(backupPath);
    write_to_file(hFile, new_content);
    unlock_file(hFile);
    free(file_contents2);
    free(backupPath);


    free(threads);
    free(filePATH);
    free(txt2);
    
    free(attributes);
    for(int j = 0; j<lenAgg; j++) {
        free(trees0[j].attrP);
        free(trees0[j].before);
        free(trees0[j].folCat);
    }
    free(trees0);
    free(temp);
}