#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>


char* name = NULL;
FILE* clipboard = NULL;
char* command = NULL;
char* firstLine = NULL;
char** files = NULL;
DIR *dir = NULL;


void freeVar(void *var) {
    if(var != NULL) {
        free(var);
    }
}

void freeAll(char* msg) {
    freeVar(command);
    freeVar(files);
    fprintf(stderr, "%s\n", msg);
    exit(0);
}

int main() {
    size_t size = 0;
    clipboard = popen("powershell.exe Get-Clipboard", "r");
    if (clipboard == NULL) {
        pclose(clipboard);
        freeAll("Failed to open clipboard.\n");
    }
    if (getline(&firstLine, &size, clipboard) == -1) {
        pclose(clipboard);
        freeAll("Error reading clipboard.\n");
    }
    name = strtok(firstLine, "\t");
    pclose(clipboard);
    int dataInClip = 0;
    int count = 0;
    if(strcmp(name, "dataSpreadSheet") == 0) {
        while (name != NULL && strcmp(name, "\r\n") != 0) {
            if(count == 3) {
                name = strtok(NULL, "\r");
                char* command0 = "powershell.exe Get-Clipboard > data/";
                char* ptr = name;
                while((ptr = strstr(ptr, " ")) != NULL) {
                    memcpy(ptr, "_", 1);
                    ptr++;
                }
                command = malloc(strlen(command0) + strlen(name) + 5);
                sprintf(command, "%s%s.txt", command0, name);
                int status = system(command);
                if(status != 0) {
                    freeAll("Failed to transfer the data.\n");
                }
                dataInClip = 1;
                break;
            }
            name = strtok(NULL, "\t");
            count++;
        }
    }
    if(!dataInClip) {
        struct dirent *ent;
        if ((dir = opendir ("data/")) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    if (strstr(ent->d_name, ".txt") != NULL && strstr(ent->d_name, "_sorted.txt") == NULL) {
                        files = realloc(files, (count+1) * sizeof(char*));
                        files[count] = strdup(ent->d_name);
                        printf("%d: %s\n", count+1, files[count]);
                        count++;
                    }
                }
            }
            closedir (dir);
        } else {
            closedir (dir);
            freeAll("Failed to open directory");
        }
        if(count==0) {
            freeAll("No data found.");
        }
        while(1) {
            if (getline(&name, &size, stdin) == -1) {
                freeAll("Error reading input");
            }
            int choice = atoi(name);
            if(choice > 0 && choice <= count) {
                name = strndup(files[choice-1], strlen(files[choice-1])-4);
                break;
            }
            printf("Invalid input.\n");
        }
    }
    char* command2 = "./sortNotes ";
    command = malloc(strlen(command2) + strlen(name)+1);
    sprintf(command, "%s%s", command2, name);
    int status = system(command);
    if(status != 0) {
        freeAll("Failed to sort notes.\n");
    }
    freeAll("");
}