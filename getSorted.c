#include "mediaSorter.h"
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

char* dataPATH = "data/";
char* sortedExtention = "_sorted";

int main() {

    
    char* name = askFile("");
    if(name == NULL) {
        return -1;
    }
    // char* name = malloc(20);
    // strcpy(name, "dance_Feuille_1");

    int lenAgg;
    char** output = NULL;
    char* txt = NULL;
    char* line = NULL;
    char** values = NULL;
    char* token = NULL;
    int** linesRef = NULL;
    int* linesRefNb = NULL;
    int lenVal;

    char* filePATH = strdup(name);
    filePATH = realloc(filePATH, strlen(name) + strlen(dataPATH) + 5);
    sprintf(filePATH, "%s%s.txt", dataPATH, name);

    


    FILE* file;
    file = fopen(filePATH, "r");
    if (file == NULL) {
        printf("Failed to open the data file.");
        return -1;
    }
    int filNo = fileno(file);
    if (flock(filNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock for the input file");
        return -1;
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
    int txtSize = strlen(txt) + 10;
    for(int i = 0; i<lenAgg; i++) {
        output[i] = strdup(values[linesRef[i][0]]);
        for(int j = 1; j<linesRefNb[i]; j++) {
            output[i] = realloc(output[i], strlen(output[i])+strlen(values[linesRef[i][j]])+1);
            strcat(output[i], values[linesRef[i][j]]);
        }
        txtSize += strlen(output[i]);
    }

    filePATH = realloc(filePATH, strlen(filePATH) + strlen(sortedExtention) + 1);
    sprintf(filePATH, "%s%s%s.txt", dataPATH, name, sortedExtention);
    // 
    file = fopen(filePATH, "r");
    if (file == NULL) {
        printf("Failed to open the sorted file.");
        return -1;
    }
    int fileNo = fileno(file);
    if (flock(fileNo, LOCK_EX) == -1) {
        printf("Failed to obtain lock for the sorted file");
        return -1;
    }
    getline(&line, &len, file);
    if (line == NULL) {
        printf("Failed to read the first line of the sorted file.");
        return -1;
    }
    flock(fileNo, LOCK_UN);
    fclose(file);
    token = strtok(line, " ");
    char* error = malloc(strlen(token)+1);
    strcpy(error, token);
    token = strtok(NULL, " ");
    char* loner = malloc(strlen(token)+1);
    strcpy(loner, token);
    token = strtok(NULL, ",");
    count = 0;
    txtSize += strlen(error) + strlen(loner) + 2;
    txt = realloc(txt, txtSize);
    while(token!=NULL && token[0] != '\r') {
        int i = atoi(token);
        if(i>=lenAgg) {
            printf("Not sorted yet.");
            return -1;
        }
        strcat(txt, output[i]);
        token = strtok(NULL, ",");
        count++;
    }
    if(count!=lenAgg) {
        printf("Not sorted yet.");
        return -1;
    }
    token = strtok(txt, "\r\n");
    char* newTxt = malloc(txtSize+strlen(error)+strlen(loner)+10);
    strcpy(newTxt, token);
    strcat(newTxt, "\r\n");
    token = strtok(NULL, "\r\n");
    strcat(newTxt, token);
    strcat(newTxt, "\t\t");
    strcat(newTxt, error);
    strcat(newTxt, "\r\n");
    token = strtok(NULL, "\r\n");
    strcat(newTxt, token);
    strcat(newTxt, "\t\t");
    strcat(newTxt, loner);
    token = strtok(NULL, "\0");
    strcat(newTxt, "\r\n");
    strcat(newTxt, token);
    FILE *clipboard = popen("clip.exe", "w");
    if (clipboard == NULL) {
        printf("Error opening clipboard");
        return -1;
    }
    // return 1;
    fprintf(clipboard, "%s", newTxt);
    pclose(clipboard);
}