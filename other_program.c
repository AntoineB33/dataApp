#include <stdio.h>

int main() {
    char input[100];

    // Read input from standard input
    fgets(input, sizeof(input), stdin);

    // Display the input received
    printf("Received input: %s\n", input);

    return 0;
}
