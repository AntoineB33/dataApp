#include <stdio.h>

int countDigitsUpToN(int n) {
    int count = 0;
    int length = 1;  // Length of the current digit range (starting from 1 digit)
    int multiplier = 9;  // Multiplier for the number of digits in the current range

    while (n > 0) {
        // Calculate the number of digits contributed by the current digit range
        count += (n < multiplier) ? n * length : multiplier * length;
        n -= multiplier;
        length++;
        multiplier *= 10;  // Increase the multiplier for the next digit range
    }

    return count*2-1;
}

int main() {
    int n = 100;
    printf("For n = %d, the length is: %d\n", n, countDigitsUpToN(n));  // Output: 23
    return 0;
}
