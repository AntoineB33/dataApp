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




int main() {

    

    
    int lenAgg;
    char** output = NULL;
    char* txt = NULL;
    char* line = NULL;
    char** values = NULL;
    char* token = NULL;
    int** linesRef = NULL;
    int* linesRefNb = NULL;
    int lenVal;
    char** files = NULL;
    char* filePATH = NULL;
    int fileNb = 0;
    char* command;
    FILE *clipboard = popen("clip.exe", "w");
    if (clipboard == NULL) {
        printf("Error opening clipboard");
        return -1;
    }
    char* tr = malloc(9957);
    if (tr == NULL) {
        printf("Error allocating memory");
        return -1;
    }
    strcpy(tr,"e");
    for(int i = 0; i<9956; i++){
        strcat(tr,"e");
    }
    fprintf(clipboard, "%s", tr);
    pclose(clipboard);
}