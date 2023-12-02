#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char line[] = "hey delim hey delim hey";
    char* token = strtok(line, "de");
    printf("%s\n", token);
    token += strlen(token)+1;
    printf("%s\n", token);

    return 0;
}
