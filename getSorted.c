#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/file.h>
#include <dirent.h>


int lenAgg;
char** output = NULL;
char* txt = NULL;
char* line = NULL;
char** values = NULL;
char* token = NULL;
int** linesRef = NULL;
int* linesRefNb = NULL;
int lenVal;
char* name = NULL;
char** files = NULL;
char* filePATH = NULL;
int fileNb = 0;
char* command;


void freeVar(void *var) {
    if(var != NULL) {
        free(var);
    }
}

void freeList(char **var, int len) {
    if(var != NULL) {
        for(int i = 0; i<len; i++) {
            free(var[i]);
        }
        free(var);
    }
}

void freeAll(char* msg) {
    freeVar(txt);
    freeVar(line);
    freeVar(linesRefNb);
    freeVar(name);
    freeVar(command);
    freeList(files, fileNb);
    freeList(output, lenAgg);
    freeList(values, lenVal);
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

int main() {

    
    size_t size = 0;
    struct dirent *ent;
    DIR *dir;
    if ((dir = opendir ("data/")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                if (strstr(ent->d_name, ".txt") != NULL && strstr(ent->d_name, "_sorted.txt") == NULL) {
                    files = realloc(files, (fileNb+1) * sizeof(char*));
                    ent->d_name[strlen(ent->d_name)-4] = '\0';
                    files[fileNb] = strdup(ent->d_name);
                    printf("%d: %s\n", fileNb+1, files[fileNb]);
                    fileNb++;
                }
            }
        }
        closedir (dir);
    } else {
        closedir (dir);
        freeAll("Failed to open directory");
    }
    if(fileNb==0) {
        freeAll("No data found.");
    }
    while(1) {
        if (getline(&name, &size, stdin) == -1) {
            freeAll("Error reading input");
        }
        int choice = atoi(name);
        if(choice > 0 && choice <= fileNb) {
            name = strdup(files[choice-1]);
            break;
        }
        printf("Invalid input.\n");
    }




    filePATH = malloc(strlen(name) + 20);
    sprintf(filePATH, "data/%s.txt", name);

    


    FILE* file = fopen(filePATH, "r");


    
    // filePATH = realloc(filePATH, strlen(name) + 20);
    // sprintf(filePATH, "data/dance_Feuille_1_sorted.txt");
    // file = fopen(filePATH, "r");
    // fclose(file);
    // sprintf(filePATH, "data/test_Feuille_1_sorted.txt");
    // file = fopen(filePATH, "r");
    // fclose(file);
    // sprintf(filePATH, "data/dance_Feuille_1_sorted.txt");
    // file = fopen(filePATH, "r");
    // fclose(file);
    // sprintf(filePATH, "data/test_Feuille_1_sorted.txt");
    // file = fopen(filePATH, "r");
    // fclose(file);


    
    if (file == NULL) {
        freeAll("Failed to open the data file.");
    }
    int filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        freeAll("Failed to obtain lock for the input file");
    }
    
    
    size_t len = 0;
    getline(&line, &len, file);
    if(line[0]=='"') {
        line++;
    }
    token = strtok(line, "\t");
    token = strtok(NULL, "\t");
    lenAgg = atoi(token);
    token = strtok(NULL, "\t");
    lenVal = atoi(token);
    getline(&line, &len, file);
    linesRef = malloc(lenAgg * sizeof(int*));
    int mediaNb = 0;
    linesRefNb = malloc(lenAgg * sizeof(int*));
    int count = 0;
    for(int i = 0; i<lenAgg; i++) {
        getline(&line, &len, file);
        linesRef[i] = malloc(lenVal * sizeof(int));
        token = strtok(line, ",");
        count = 0;
        while(token!=NULL && token[0] != '\r') {
            linesRef[i][count] = atoi(token)-1;
            token = strtok(NULL, ",");
            count++;
        }
        linesRefNb[i] = count;
        getline(&line, &len, file);
        getline(&line, &len, file);
        getline(&line, &len, file);
    }

    getline(&line, &len, file);
    txt = strdup(line);
    values = malloc(lenVal * sizeof(char*));
    for(int i = 0; i<lenVal; i++) {
        getline(&line, &len, file);
        values[i] = strdup(line);
    }
    flock(filNo, LOCK_UN);
    fclose(file);
    if(values[lenVal-1][strlen(values[lenVal-1])-1]=='"') {
        values[lenVal-1][strlen(values[lenVal-1])-1]='\0';
        strcat(values[lenVal-1], "\r\n");
    }
    output = malloc(lenAgg * sizeof(char*));
    int txtSize = strlen(txt);
    for(int i = 0; i<lenAgg; i++) {
        output[i] = strdup(values[linesRef[i][0]]);
        for(int j = 1; j<linesRefNb[i]; j++) {
            output[i] = realloc(output[i], strlen(output[i])+strlen(values[linesRef[i][j]]));
            strcat(output[i], values[linesRef[i][j]]);
        }
        txtSize += strlen(output[i]);
    }
    txt = realloc(txt, txtSize);

    sprintf(filePATH, "data/%s_sorted.txt", name);
    file = fopen(filePATH, "r");
    if (file == NULL) {
        freeAll("Failed to open the sorted file.");
    }
    int fileNo = fileno(file);
    if (flock(fileNo, LOCK_EX) == -1) {
        freeAll("Failed to obtain lock for the sorted file");
    }
    getline(&line, &len, file);
    if (line == NULL) {
        freeAll("Failed to read the first line of the sorted file.");
    }
    flock(fileNo, LOCK_UN);
    fclose(file);
    token = strtok(line, ",");
    count = 0;
    while(token!=NULL && token[0] != '\r') {
        int i = atoi(token);
        if(i>=lenAgg) {
            freeAll("Not sorted yet.");
        }
        strcat(txt, output[i]);
        token = strtok(NULL, ",");
        count++;
    }
    if(count!=lenAgg) {
        freeAll("Not sorted yet.");
    }
    FILE *clipboard = popen("clip.exe", "w");
    if (clipboard == NULL) {
        freeAll("Error opening clipboard");
    }

    // Write the text to the clipboard
    fprintf(clipboard, "%s", txt);

    // Close the pipe
    if (pclose(clipboard) != 0) {
        freeAll("Error closing clipboard");
    }
    freeAll("Success");
}