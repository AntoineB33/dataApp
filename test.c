#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char input[] = "1,2,3\n4,5,6,7\n8,9"; // Your input string

    char* tokenSup = strtok(input, "\n");
    char* line = malloc(100);
    strcpy(line,tokenSup);
    char* token = strtok(line, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    token = strtok(NULL, ",");
    tokenSup = strtok(input, "\n");
    token = strtok(line, ",");

    return 0;
}
