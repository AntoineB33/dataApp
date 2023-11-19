#include "common_header.h"
#include <stdio.h>

int h = 0;

void functionFromScript1() {
    printf("Function from script1%d\n",h);
}

int main() {
    // Call function from script1
    functionFromScript2();
    return 0;
}
