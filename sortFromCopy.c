#include "mediaSorter.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>




int main() {
    
    char* name;
    FILE* clipboard;
    char* command;
    char* firstLine;
    DIR *dir;

    size_t size = 0;
    clipboard = popen("powershell.exe Get-Clipboard", "r");
    if (clipboard == NULL) {
        pclose(clipboard);
        printf("Failed to open clipboard.\n");
        return -1;
    }
    if (getline(&firstLine, &size, clipboard) == -1) {
        pclose(clipboard);
        printf("Error reading clipboard.\n");
        return -1;
    }
    name = strtok(firstLine, "\t");
    pclose(clipboard);
    int dataInClip = 0;
    int count = 0;
    if(strcmp(name, "dataSpreadSheet") == 0) {
        while (name != NULL && strcmp(name, "\r\n") != 0) {
            if(count == 5) {
                name = strtok(NULL, "\r");
                char* command0 = "powershell.exe Get-Clipboard > data/";
                char* ptr = name;
                while((ptr = strstr(ptr, " ")) != NULL) {
                    memcpy(ptr, "_", 1);
                    ptr++;
                }
                command = malloc(strlen(command0) + strlen(name) + 10);
                sprintf(command, "%s%s.txt", command0, name);
                int status = system(command);
                if(status != 0) {
                    printf("Failed to transfer the data.\n");
                    return -1;
                }
                free(command);
                dataInClip = 1;
                break;
            }
            name = strtok(NULL, "\t");
            count++;
        }
    }
    if(!dataInClip) {
        name = askFile("");
        if(name==NULL) {
            printf("Failed to get file name.\n");
            return -1;
        }
    }
    initSort(name);
}