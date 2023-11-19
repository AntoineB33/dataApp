#include "mediaSorter.h"
#include <stdio.h>


int main(int argc, char *argv[]) {
    if(argv[1] != NULL) {
        initSort(argv[1]);
    } else {
        printf("No input file specified.\n");
    }
}