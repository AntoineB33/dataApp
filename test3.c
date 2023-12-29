#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>




int main() {
    char* clipboard00 = "dataSpreadSheet	192	338	48	dance_Feuille 1";
    char* clipboard0 = malloc(strlen(clipboard00) + 1);
    strcpy(clipboard0, clipboard00);
    int dataInClip = 0;
    char* name;
    if(clipboard0!=NULL) {
        char* clipboard = malloc(strlen(clipboard0) + 1);
        strcpy(clipboard, clipboard0);
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
                    char* filePATH = malloc(strlen(name0) + 5);
                    snprintf(filePATH, 1024, "data/%s.txt", name0);
                    dataInClip = 1;
                    printf("%ld\n", strlen(name0) + 1);
                    // char* try = malloc(strlen(name0) + 1);
                    // (void)try;
                    printf("name0: %s\n", name0);
                    name = malloc(16);
                    strcpy(name, name0);
                    break;
                }
                name0 = strtok(NULL, "\t");
                count++;
            }
        }
        free(clipboard);
        free(clipboard0);
    }
    (void)dataInClip;
    printf("Done.\n");
    sleep(1);
}