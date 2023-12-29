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
                if (strstr(ent->d_name, ".txt") != NULL && strstr(ent->d_name, "_sorted.txt") == NULL && strstr(ent->d_name, "_backup.txt") == NULL) {
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