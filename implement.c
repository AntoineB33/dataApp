#include "mediaSorter.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/select.h>


char* askFile(char* path0) {
    char* name = NULL;
    char** files = NULL;
    DIR *dir;
    size_t size = 0;
    int count = 0;
    struct dirent *ent;
    char* path = malloc(strlen(path0) + 10);
    sprintf(path, "%sdata/", path0);
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                if (strstr(ent->d_name, ".txt") != NULL && strstr(ent->d_name, "_sorted.txt") == NULL) {
                    files = realloc(files, (count+1) * sizeof(char*));
                    files[count] = strdup(ent->d_name);
                    printf("%d: %s\n", count+1, strndup(files[count], strlen(files[count])-4));
                    count++;
                }
            }
        }
        closedir (dir);
    } else {
        closedir (dir);
        printf("Failed to open directory");
        return NULL;
    }
    if(count==0) {
        printf("No data found.");
        return NULL;
    }
    while(1) {
        if (getline(&name, &size, stdin) == -1) {
            printf("Error reading input");
            return NULL;
        }
        int choice = atoi(name);
        if(choice > 0 && choice <= count) {
            name = strndup(files[choice-1], strlen(files[choice-1])-4);
            break;
        }
        printf("Invalid input.\n");
    }
    for(int i = 0; i<count; i++) {
        free(files[i]);
    }
    free(files);
    free(path);
    return name;
}

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
pthread_rwlock_t lonerM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t errorWM = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t lonerWM = PTHREAD_RWLOCK_INITIALIZER;
pthread_t *threads = NULL;
FILE* file = NULL;
char* filePATH = NULL;
size_t len;
int lastThRoot;
char* temp = NULL;


void* sortTable() {
    // intptr_t threadId = (intptr_t)id;
    int lenAggP = lenAgg+1;
    treeCons* trees = malloc(lenAgg * sizeof(treeCons));
    for(int i = 0; i<lenAgg; i++) {
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
    }
    int* res = malloc(lenAggP * sizeof(int));
    int* spaces = malloc((lenAggP) * sizeof(int));
    int* loners = malloc((lenAggP) * sizeof(int));
    int* errori = malloc((lenAggP) * sizeof(int));
    spaces[0] = 0;
    loners[0] = 0;
    errori[0] = 0;
    attrProp* y = NULL;
    attribute* attrP = NULL;
    attribute* attributesI = malloc(attNb * sizeof(attribute));
    for(int j = 0; j<attNb; j++) {
        attributesI[j].last = attributes[j].last;
        attributesI[j].dist = attributes[j].dist;
        attributesI[j].rest = attributes[j].rest;
        attributesI[j].prevSt = malloc(attributes[j].prevSize * sizeof(prevStT));
        attributesI[j].prevSize = 0;
    }
    int i = 1;  // ith node to place
    int imm = 0;    // i-1
    int n = 0;  // nth medium
    int c;
    int back = 0;
    treeCons* po = NULL;
    int errorP;
    int lonerP;
    prevStT* prevIP = NULL;
    int lastThRootP = -1;
    while(1) {
        pthread_rwlock_wrlock(&newRoot);
        while(1) {
            while(i>0 && i<=lvl) {
                if(back==0) {
                    res[i] = 0;
                } else {
                    po = &trees[res[i]];
                    for(int k = 0; k<po->attrPSize; k++) {
                        attrP = &attributesI[po->attrP[k].attr];
                        if(attrP->last!=-2) {
                            prevIP = &attrP->prevSt[--attrP->prevSize];
                            attrP->last = prevIP->prevLast;
                            attrP->rest = prevIP->prevRest;
                        }
                    }
                    if(back==2) {
                        imm--;
                        n-=trees[res[i]].mediaSize;
                        trees[res[i]].afters--;
                        for(int k = 0; k<trees[res[i]].befSize; k++) {
                            trees[trees[res[i]].before[k]].afters++;
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
                    attrP = &attributesI[y->attr];
                    if(attrP->last==-2) {
                        c = attrP->dist-n;
                        if(c>0) {
                            loners[i] += c;
                        } else if(c<0) {
                            loners[i] -= c;
                            if(attrP->rest>0) {
                                loners[i] += c-1;
                            } else {
                                loners[i] += c;
                            }
                        }
                    } else {
                        prevIP = &attrP->prevSt[attrP->prevSize++];
                        prevIP->prevLast = attrP->last;
                        prevIP->prevRest = attrP->rest;
                        if(attrP->last==-1) {
                            errori[i] += n+y->posInt;
                            attrP->last = n+y->posOut-1;
                        } else {
                            c = attrP->dist-n+attrP->last-y->posInt;
                            if(c>0) {
                                if(attrP->rest>0) {
                                    errori[i] += c-1;
                                    attrP->rest--;
                                } else {
                                    errori[i] += c;
                                }
                            } else {
                                spaces[i]-=c;
                            }
                            attrP->last = n+y->posOut-1;
                        }
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
                trees[res[i]].afters++;
                for(int k = 0; k<trees[res[i]].befSize; k++) {
                    trees[trees[res[i]].before[k]].afters--;
                }
                n+=trees[res[i]].mediaSize;
                i++;
                imm++;
                back = 0;
            }
            if(i==0) {
                pthread_rwlock_unlock(&newRoot);
                for(int j = 0; j<attNb; j++) {
                    free(attributesI[j].prevSt);
                }
                free(attributesI);
                free(res);
                free(spaces);
                free(loners);
                free(errori);
                for(int j = 0; j<lenAgg; j++) {
                    free(trees[j].attrP);
                    free(trees[j].before);
                }
                free(trees);
                return NULL;
            }
            lastThRootP++;
            if(lastThRootP>lastThRoot) {
                break;
            }
            i--;
            back = 2;
        }
        lastThRoot = lastThRootP;
        // printf("lastThRoot:%d %ld\n", lastThRoot, threadId);
        pthread_rwlock_unlock(&newRoot);
        while (1) {
            // if(sorter(i,lenAggP, imm, n, res, spaces, loners, errori, back, attributesI, errorP, lonerP, newSort, cal, u, xc, compar, k, y, q, po, c) == -1) {
            //     return except;
            // }
            while(i>lvl && i<lenAggP) {
                if(back==0) {
                    res[i] = 0;
                } else {
                    // return NULL;
                    po = &trees[res[i]];
                    for(int k = 0; k<po->attrPSize; k++) {
                        attrP = &attributesI[po->attrP[k].attr];
                        if(attrP->last!=-2) {
                            prevIP = &attrP->prevSt[--attrP->prevSize];
                            attrP->last = prevIP->prevLast;
                            attrP->rest = prevIP->prevRest;
                        }
                    }
                    // return NULL;
                    if(back==2) {
                        imm--;
                        n-=trees[res[i]].mediaSize;
                        trees[res[i]].afters--;
                        for(int k = 0; k<trees[res[i]].befSize; k++) {
                            trees[trees[res[i]].before[k]].afters++;
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
                    attrP = &attributesI[y->attr];
                    if(attrP->last==-2) {
                        c = attrP->dist-n;
                        if(c>0) {
                            loners[i] += c;
                        } else if(c<0) {
                            loners[i] -= c;
                            if(attrP->rest>0) {
                                loners[i] += c-1;
                            } else {
                                loners[i] += c;
                            }
                        }
                    } else {
                        prevIP = &attrP->prevSt[attrP->prevSize++];
                        prevIP->prevLast = attrP->last;
                        prevIP->prevRest = attrP->rest;
                        if(attrP->last==-1) {
                            errori[i] += n+y->posInt;
                            attrP->last = n+y->posOut-1;
                        } else {
                            c = attrP->dist-n+attrP->last-y->posInt;
                            if(c>0) {
                                if(attrP->rest>0) {
                                    errori[i] += c-1;
                                    attrP->rest--;
                                } else {
                                    errori[i] += c;
                                }
                            } else {
                                spaces[i]-=c;
                            }
                            attrP->last = n+y->posOut-1;
                        }
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
                trees[res[i]].afters++;
                for(int k = 0; k<trees[res[i]].befSize; k++) {
                    trees[trees[res[i]].before[k]].afters--;
                }
                n+=trees[res[i]].mediaSize;
                i++;
                imm++;
                back = 0;
            }
            // return NULL;
            if(i==lvl) {
                break;
            }
            i--;
            pthread_rwlock_wrlock(&checkM);
            pthread_rwlock_rdlock(&errorWM);
            if(errori[i]<error) {
                pthread_rwlock_wrlock(&errorM);
                // error = errori[i];
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
            // pthread_rwlock_unlock(&checkM);
            // return NULL;
            if(back) {
                pthread_rwlock_wrlock(&fileM);
                pthread_rwlock_unlock(&checkM);
                // printf("start\n");
                sprintf(txt2, "%d %d ", errori[i], loners[i]);
                // pthread_rwlock_unlock(&fileM);
                // return NULL;
                for(int j = 1; j<lenAggP; j++) {
                    sprintf(temp, "%d,", res[j]);
                    strcat(txt2, temp);
                }
                file = fopen(filePATH, "w");
                // pthread_rwlock_unlock(&fileM);
                // return NULL;
                if (file == NULL) {
                    printf("Failed to open the output file.\n");
                    pthread_rwlock_unlock(&fileM);
                    return NULL;
                }
                int fileNo = fileno(file);
                if (flock(fileNo, LOCK_EX) == -1) {
                    printf("Failed to obtain lock\n");
                    pthread_rwlock_unlock(&fileM);
                    return NULL;
                }
                fprintf(file, "%s", txt2);
                flock(fileNo, LOCK_UN);
                fclose(file);
                // printf("end\n");
                pthread_rwlock_unlock(&fileM);
                // return NULL;
            } else{
                pthread_rwlock_unlock(&checkM);
            }
            back = 2;
        }
    }
    return NULL;
}

bool waitsInput = true;

void *userInterr() {
    printf("Press q to stop the sorting.\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fd_set fds;
    struct timeval timeout;

    while (waitsInput) {
        FD_ZERO(&fds);
        FD_SET(fileno(stdin), &fds);

        timeout.tv_sec = 5;  // Set timeout to 5 seconds
        timeout.tv_usec = 0;

        int ready = select(fileno(stdin) + 1, &fds, NULL, NULL, &timeout);

        if (ready == -1) {
            perror("select");
            break;
        } else if (ready == 0) {
            continue; // Continue waiting for input
        } else {
            read = getline(&line, &len, stdin);
            if (read != -1) {
                if(strcmp(line, "q\n") == 0) {
                    pthread_rwlock_wrlock(&errorM);
                    errorP = error;
                    error = -1;
                    pthread_rwlock_unlock(&errorM);
                    break;
                }
            }
        }
    }

    free(line); // Don't forget to free allocated memory
    if(waitsInput) {
        waitsInput = false;
    }
    pthread_exit(NULL);
}
// void* userInterr() {
//     printf("Press q to stop the sorting.\n");
//     char *input = NULL;
//     while(1) {
//         if (getline(&input, &len, stdin) == -1) {
//             printf("Error reading input");
//             return NULL;
//         }
//         if(strcmp(input, "q\n") == 0) {
//             pthread_rwlock_wrlock(&errorM);
//             error = -1;
//             pthread_rwlock_unlock(&errorM);
//             break;
//         }
//     }
//     free(input);
//     waitsInput = false;
//     return NULL;
// }

int initSort(char *argv) {

    error = INT_MAX;
    space = 0;
    loner = INT_MAX;
    filePATH = malloc(strlen(argv) + 10);
    sprintf(filePATH, "data/%s.txt", argv);
    lenAgg = 0;



    
    char *file_contents;
    long file_size;

    // Open the file in binary read mode
    file = fopen(filePATH, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    int filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock");
        return -1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory to store the file contents
    file_contents = malloc(file_size * sizeof(char));
    if (file_contents == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // Read the file contents into the allocated memory
    fread(file_contents, sizeof(char), file_size, file);
    flock(filNo, LOCK_UN);
    fclose(file);

    // Ensure proper termination of the char* as a string
    file_contents[file_size - 1] = '\0';
    char* file_contents_orig = file_contents;
    if(file_contents[0]=='"') {
        file_contents++;
    }
    char* endChars = "\r\nresult.txt\r\nAffichage de result.txt\r\n";
    if(strcmp(file_contents+strlen(file_contents)-strlen(endChars), endChars) == 0) {
        file_contents[strlen(file_contents)-strlen(endChars)] = '\0';
    }
    


    
    char *file_backup;
    sprintf(filePATH, "data/%s_backup.txt", argv);
    // Open the file in binary read mode
    file = fopen(filePATH, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock");
        return -1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory to store the file contents
    file_backup = malloc(file_size * sizeof(char));
    if (file_backup == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    // Read the file contents into the allocated memory
    fread(file_backup, sizeof(char), file_size, file);
    flock(filNo, LOCK_UN);
    fclose(file);

    // Ensure proper termination of the char* as a string
    file_backup[file_size - 1] = '\0';
    char* file_backup_orig = file_backup;
    if(file_backup[0]=='"') {
        file_backup++;
    }
    if(strcmp(file_backup+strlen(file_backup)-strlen(endChars), endChars) == 0) {
        file_backup[strlen(file_backup)-strlen(endChars)] = '\0';
    }
    char* token = strtok(file_backup, "\n");
    if(token!=NULL) {
        int errorPot = atoi(token);
        token += strlen(token)+1;
        if(strcmp(file_contents, token) == 0) {
            error = errorPot;
        }
    }

    // Clean up: Close the file and free allocated memory
    free(file_backup_orig);





    file = fopen(filePATH, "r");
    if (file == NULL) {
        filePATH = realloc(filePATH, strlen(filePATH) + 30);
        printf("Failed to open the file %s\n",filePATH);
        return -1;
    }
    filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock");
        return -1;
    }
    
    
    len = 0;
    token = strtok(file_contents, "\t");
    token = strtok(NULL, "\t");
    lenAgg = atoi(token);
    token = strtok(NULL, "\t");
    int lenVal = atoi(token);
    token = strtok(NULL, "\t");
    attNb = atoi(token);
    token = strtok(NULL, "\r");
    int number[2] = {0,0};
    attributes = malloc(attNb * sizeof(attribute));
    int matrixSize = lenAgg*4+lenVal+2;
    char** matrix = malloc(matrixSize * sizeof(char*));
    for(int i = 0; i<matrixSize; i++) {
        token = strtok(NULL,"\r");
        token++;
        matrix[i] = malloc(strlen(token)+1);
        strcpy(matrix[i], token);
        // printf("%s\n", matrix[i]);
    }
    token = strtok(matrix[0], ",");
    for(int i = 0; i<attNb; i++) {
        attributes[i].prevSize = atoi(token);
        if(attributes[i].prevSize>0) {
            number[0]+=attributes[i].prevSize;
        } else {
            number[1]++;
        }
        token = strtok(NULL, ",");
    }
    int count;
    trees0 = malloc(lenAgg * sizeof(treeCons));
    int** precRef = malloc(lenAgg * sizeof(int*));
    int mediaNb = 0;
    for(int i = 0; i<lenAgg; i++) {
        token = strtok(matrix[i*4+2], ",");
        trees0[i].afters = atoi(token);
        token = strtok(NULL, "");
        trees0[i].mediaSize = atoi(token);
        mediaNb += trees0[i].mediaSize;
        count = 0;
        precRef[i] = malloc(lenAgg * sizeof(int));
        token = strtok(matrix[i*4+3], ",");
        while(token!=NULL) {
            precRef[i][count] = atoi(token);
            count++;
            token = strtok(NULL, ",");
        }
        trees0[i].befSize = count;

        trees0[i].attrP = malloc(attNb * sizeof(attrProp));
        count = 0;
        token = strtok(matrix[i*4+4], ",");
        while(token!=NULL) {
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
    free(file_contents_orig);
    flock(filNo, LOCK_UN);
    fclose(file);
    for(int i = 0; i<lenAgg; i++) {
        trees0[i].before = malloc(trees0[i].befSize * sizeof(treeCons*));
        for(int j = 0; j<trees0[i].befSize; j++) {
            trees0[i].before[j] = precRef[i][j];
        }
    }
    for(int i = 0; i<lenAgg; i++) {
        free(precRef[i]);
    }
    free(precRef);
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
    int numCores = sysconf(_SC_NPROCESSORS_ONLN);
    numCores = 2;
    threads = malloc((numCores+1) * sizeof(pthread_t));
    lvl = 0;

    filePATH = realloc(filePATH, strlen(filePATH) + 20);
    sprintf(filePATH, "data/%s_sorted.txt", argv);


    
    
    int lenAggP = lenAgg+1;
    treeCons* trees = malloc(lenAgg * sizeof(treeCons));
    for(int i = 0; i<lenAgg; i++) {
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
    }
    int* res = malloc(lenAggP * sizeof(int));
    int* spaces = malloc((lenAggP) * sizeof(int));
    int* loners = malloc((lenAggP) * sizeof(int));
    int* errori = malloc((lenAggP) * sizeof(int));
    spaces[0] = 0;
    loners[0] = 0;
    errori[0] = 0;
    attrProp* y;
    attribute* attrP;
    attribute* attributesI = malloc(attNb * sizeof(attribute));
    for(int j = 0; j<attNb; j++) {
        attributesI[j].last = attributes[j].last;
        attributesI[j].dist = attributes[j].dist;
        attributesI[j].rest = attributes[j].rest;
        attributesI[j].prevSt = malloc(attributes[j].prevSize * sizeof(prevStT));
        attributesI[j].prevSize = 0;
    }
    int i = 1;  // ith node to place
    int imm = 0;    // i-1
    int n = 0;  // nth medium
    int c;
    int back = 0;
    treeCons* po;
    lvl = 1;
    int nbLeaves = 0;
    int nbLeavesi = 0;
    prevStT* prevIP;
    while(1) {
        while(i>0 && i<=lvl) {
            if(back==0) {
                res[i] = 0;
            } else {
                po = &trees[res[i]];
                for(int k = 0; k<po->attrPSize; k++) {
                    attrP = &attributesI[po->attrP[k].attr];
                    if(attrP->last!=-2) {
                        prevIP = &attrP->prevSt[--attrP->prevSize];
                        attrP->last = prevIP->prevLast;
                        attrP->rest = prevIP->prevRest;
                    }
                }
                if(back==2) {
                    imm--;
                    n-=trees[res[i]].mediaSize;
                    trees[res[i]].afters--;
                    for(int k = 0; k<trees[res[i]].befSize; k++) {
                        trees[trees[res[i]].before[k]].afters++;
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
                attrP = &attributesI[y->attr];
                if(attrP->last==-2) {
                    c = attrP->dist-n;
                    if(c>0) {
                        loners[i] += c;
                    } else if(c<0) {
                        loners[i] -= c;
                        if(attrP->rest>0) {
                            loners[i] += c-1;
                        } else {
                            loners[i] += c;
                        }
                    }
                } else {
                    prevIP = &attrP->prevSt[attrP->prevSize++];
                    prevIP->prevLast = attrP->last;
                    prevIP->prevRest = attrP->rest;
                    if(attrP->last==-1) {
                        errori[i] += n+y->posInt;
                        attrP->last = n+y->posOut-1;
                    } else {
                        c = attrP->dist-n+attrP->last-y->posInt;
                        if(c>0) {
                            if(attrP->rest>0) {
                                errori[i] += c-1;
                                attrP->rest--;
                            } else {
                                errori[i] += c;
                            }
                        } else {
                            spaces[i]-=c;
                        }
                        attrP->last = n+y->posOut-1;
                    }
                }
            }
            if(errori[i]>error) {
                back = 1;
                continue;
            }
            if(errori[i]==error) {
                if(loners[i]>=loner) {
                    back = 1;
                    continue;
                }
            }
            trees[res[i]].afters++;
            for(int k = 0; k<trees[res[i]].befSize; k++) {
                trees[trees[res[i]].before[k]].afters--;
            }
            n+=trees[res[i]].mediaSize;
            i++;
            imm++;
            back = 0;
        }
        if(i==0) {
            if(lenAgg==lvl) {
                numCores = nbLeavesi;
                if(lenAgg==0) {
                    numCores = 1;
                }
                break;
            }
            nbLeavesi = nbLeaves;
            nbLeaves = 0;
            lvl++;
        }
        nbLeaves++;
        if(nbLeaves==numCores) {
            break;
        }
        back = 2;
    }
    for(int j = 0; j<attNb; j++) {
        free(attributesI[j].prevSt);
    }
    free(attributesI);
    free(res);
    free(spaces);
    free(loners);
    free(errori);
    for(int j = 0; j<lenAgg; j++) {
        free(trees[j].attrP);
        free(trees[j].before);
    }
    free(trees);
    lastThRoot = -1;
    printf("lvl:%d\n", lvl);

    int txtSize = 0;
    int length = 1;  // Length of the current digit range (starting from 1 digit)
    int multiplier = 9;  // Multiplier for the number of digits in the current range
    n = lenAgg;
    int tempSize = 0;
    if(n==0) {
        txtSize = 1;
    } else {
        while (n > 0) {
            // Calculate the number of digits contributed by the current digit range
            tempSize += (n < multiplier) ? n * length : multiplier * length;
            n -= multiplier;
            length++;
            multiplier *= 10;  // Increase the multiplier for the next digit range
        }
        txtSize = 2*tempSize;
    }
    for(int i = 0; i<2; i++) {
        length = 0;
        number[i] *= lenVal;
        // If the number is zero, it has one digit
        if (number[i] == 0) {
            length++;
        } else {
            while (number[i] != 0) {
                length++;
                number[i] /= 10;
            }
        }
        txtSize += length*2+1;
    }
    temp = malloc(tempSize+2);
    txt2 = malloc(txtSize+1);
    // error = -1;
    for(int i = 0; i<numCores; i++) {
        if(pthread_create(&threads[i], NULL, sortTable, NULL)!=0) {
            printf("Error creating thread %d.\n",i);
            return -1;
        }
    }
    if(pthread_create(&threads[numCores], NULL, userInterr, NULL)!=0) {
        printf("Error creating thread %d.\n",i);
        return -1;
    }
    for(int i = 0; i<numCores; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("pthread_join");
            return -1;
        }
    }
    waitsInput = false;
    if (pthread_join(threads[i], NULL) != 0) {
        printf("pthread_join");
        return -1;
    }
    file = fopen(filePATH, "w");
    if (file == NULL) {
        printf("Failed to open the output file.\n");
        return -1;
    }
    int fileNo = fileno(file);
    if (flock(fileNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock\n");
        return -1;
    }
    fprintf(file, "%d\n%s", errorP, txt2);
    flock(fileNo, LOCK_UN);
    fclose(file);


    free(threads);
    free(filePATH);
    free(txt2);
    
    free(attributes);
    for(int j = 0; j<lenAgg; j++) {
        free(trees0[j].attrP);
        free(trees0[j].before);
    }
    free(trees0);
    return 1;
}



// int main() {
//     initSort("dance_Feuille_1");
//     return 1;
// }