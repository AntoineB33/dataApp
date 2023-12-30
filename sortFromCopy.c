#include "sorter.h"
#include "askFile.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/file.h>




int main() {
    char* clipboard0;
    size_t size = 0;
    FILE* file = popen("powershell.exe Get-Clipboard", "r");
    if (file == NULL) {
        pclose(file);
        printf("Failed to open clipboard.\n");
        getchar();
        return -1;
    }
    if (getline(&clipboard0, &size, file) == -1) {
        pclose(file);
        printf("Error reading clipboard.\n");
        getchar();
        return -1;
    }
    pclose(file);
    int dataInClip = 0;
    char* name;
    if(clipboard0!=NULL) {
        char* clipboard = strdup(clipboard0);
        char* name0 = strtok(clipboard0, "\t");
        int count = 0;
        if(strcmp(name0, "dataSpreadSheet") == 0) {
            while (name0 != NULL && strcmp(name0, "\r\n") != 0) {
                if(count == 3) {
                    name0 = strtok(NULL, "\r");
                    char* ptr = name0;
                    while((ptr = strstr(ptr, " ")) != NULL) {
                        memcpy(ptr, "_", 1);
                        ptr++;
                    }
                    char* filePATH = malloc(strlen(name0) + 10);
                    sprintf(filePATH, "data/%s.txt", name0);
                    file = fopen(filePATH, "w");
                    if (file == NULL) {
                        printf("Failed to open the output file.\n");
                        getchar();
                        return -1;
                    }
                    int fileNo = fileno(file);
                    if (flock(fileNo, LOCK_EX) == -1) {
                        printf("Failed to obtain lock\n");
                        getchar();
                        return -1;
                    }
                    fprintf(file, "%s", clipboard);
                    flock(fileNo, LOCK_UN);
                    fclose(file);
                    dataInClip = 1;
                    name = strdup(name0);
                    break;
                }
                name0 = strtok(NULL, "\t");
                count++;
            }
        }
        free(clipboard);
        free(clipboard0);
    }
    if(!dataInClip) {
        name = askFile("");
        if(name==NULL) {
            printf("Failed to get file name.\n");
            getchar();
            return -1;
        }
    }
    initSort(name);
    printf("Done.\n");
    sleep(1);
}