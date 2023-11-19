#include "common_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

void functionFromScript2() {
    printf("Function from script2\n");
}

int main() {
    // Call function from script2
    functionFromScript1();
    return 0;
}
