#include "example.h"
#include <stdio.h>

int main() {
    // Use the structure in the main file
    Employee emp1 = {1, "John Doe", 50000.0};
    Employee emp2 = {2, "Jane Smith", 60000.0};

    // Call a function that uses the structure
    printEmployeeInfo(emp1);
    printEmployeeInfo(emp2);

    return 0;
}
