#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *suite = malloc(10000);
    char* temp = malloc(100);
    for(int i = 0; i < 167; i++) {
        sprintf(temp, "%d,", i);
        strcat(suite,temp);
    }
    printf("%s\n%d\n", suite, strlen(suite));

    return 0;
}
