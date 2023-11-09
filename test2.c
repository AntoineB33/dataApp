#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

int main() {
    int fd;
    
    // Open the file for writing and obtain a lock
    fd = open("shared_file.txt", O_RDONLY, 0666);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }
    close(fd);
    
    return 0;
}
