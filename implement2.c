#include "mediaSorter.h"
#include <stdlib.h>
#include <stdio.h>
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


int lenAgg;
int attNb;
char* txt;

int initSort(char *argv) {

    char* error = malloc(strlen(argv) + 1);
    strcpy(error, argv);
    free(error);
    return 1;
}



int main() {
    initSort("dance_Feuille_1");
}