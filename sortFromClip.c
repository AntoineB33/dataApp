#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

int main() {
    char* clipboard_text = NULL;
    size_t clipboard_size = 0;
    FILE* clipboard = popen("powershell.exe Get-Clipboard", "r");
    if (clipboard == NULL) {
        printf("Failed to open clipboard.\n");
        return -1;
    }
    getline(&clipboard_text, &clipboard_size, clipboard);
    pclose(clipboard);
    char* fourth_word;
    int dataInClip = 0;
    char* token = strtok(clipboard_text, "\t");
    int count = 0;
    if(strcmp(token, "dataSpreadSheet") == 0) {
        while (token != NULL && strcmp(token, "\r\n") != 0) {
            if(count == 3) {
                fourth_word = strdup(token);
                dataInClip = 1;
                break;
            }
            token = strtok(NULL, "\t");
            count++;
        }
    }
    if(dataInClip) {
        strcat(fourth_word, ".txt");
        FILE* dataWorkSheet = fopen(fourth_word, "w");
        if(dataWorkSheet == NULL) {
            printf("Failed to open dataWorkSheet.txt\n");
            return -1;
        }
        fprintf(dataWorkSheet, "%s", fourth_word);
        fclose(dataWorkSheet);
    } else {
        DIR *dir;
        struct dirent *ent;
        char** files;
        if ((dir = opendir ("data/")) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    char* file_name = strdup(ent->d_name);
                    if (strstr(file_name, ".txt") != NULL) {
                        files = realloc(files, (count+1) * sizeof(char*));
                        files[count] = file_name;
                        count++;
                    }
                }
            }
            closedir (dir);
        } else {
            perror ("Failed to open directory");
            return -1;
        }
        char* input;
        int inputSize;
        if (getline(&input, &inputSize, stdin) == -1) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }
        int choice = atoi(input);
        if(choice > 0 && choice <= count) {
            fourth_word = files[choice-1];
        } else {
            printf("Invalid input.\n");
            return -1;
        }
    }
    free(clipboard_text);
    strcat(fourth_word, ".txt && ./sortNotes");
    char* command = "powershell.exe Get-Clipboard > ";
    command = realloc(command, strlen(command) + strlen(fourth_word) + 1);
    strcat(command, fourth_word);
    system(command);
    free(command);
    return 0;
}