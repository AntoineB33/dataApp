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

#define INPUT "/mnt/c/Users/comma/Documents/health/news_underground/mediaSorter/dataApp/dataWorkSheet.txt"
#define SORTF "/mnt/c/Users/comma/Documents/health/news_underground/mediaSorter/dataApp/sorted.txt"


int lenAgg;
char** output;
char* txt;
int txtSize;
FILE* file;
char* line;
char** values;
char* token;
int** linesRef;
int* linesRefNb;
int lenVal;


void freeAll(char* msg) {
    fprintf(stderr, "%s\n", msg);
    free(file);
    exit(-1);
}

void freeAll0(char* msg) {
    free(txt);
    free(line);
    free(values);
    free(token);
    free(linesRefNb);
    for(int i = 0; i<lenAgg; i++) {
        free(linesRef[i]);
    }
    for(int i = 0; i<lenVal; i++) {
        free(values[i]);
    }
    free(linesRef);
    free(values);
    freeAll(msg);
}

int main() {
    file = fopen(INPUT, "r");
    if (file == NULL) {
        freeAll("Failed to open input the file.");
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
    lenAgg = atoi(token);
    token = strtok(NULL, "\t");
    lenVal = atoi(token);
    getline(&line, &len, file);
    int count;
    linesRef = malloc(lenAgg * sizeof(int*));
    int mediaNb = 0;
    linesRefNb = malloc(lenAgg * sizeof(int*));
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
    int txtSize = 1;
    for(int i = 0; i<lenAgg; i++) {
        output[i] = strdup(values[linesRef[i][0]]);
        for(int j = 1; j<linesRefNb[i]; j++) {
            output[i] = realloc(output[i], strlen(output[i])+strlen(values[linesRef[i][j]]));
            strcat(output[i], values[linesRef[i][j]]);
        }
        txtSize += strlen(output[i]);
    }
    txt = malloc(txtSize);


    file = fopen(SORTF, "r");
    if (file == NULL) {
        freeAll0("Failed to open the sorted file.");
    }
    int fileNo = fileno(file);
    if (flock(fileNo, LOCK_EX) == -1) {
        freeAll0("Failed to obtain lock for the sorted file");
    }
    getline(&line, &len, file);
    flock(fileNo, LOCK_UN);
    fclose(file);
    token = strtok(line, ",");
    count = 0;
    while(token!=NULL && token[0] != '\r') {
        int i = atoi(token);
        if(i>=lenAgg) {
            freeAll0("Not sorted yet.");
        }
        strcat(txt, output[i]);
        token = strtok(NULL, ",");
        count++;
    }
    if(count!=lenAgg) {
        freeAll0("Not sorted yet.");
    }
    char* command = malloc(txtSize);
    sprintf(command, "/mnt/c/Windows/System32/echo.exe %s | /mnt/c/Windows/System32/clip.exe", txt); // Format the command
    system(command);
    free(command);
    freeAll0("Success");
}