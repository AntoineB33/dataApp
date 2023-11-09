#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

int main() {
    int fd;
    
    // Open the file for writing and obtain a lock
    fd = open("shared_file.txt", O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }
    if (flock(fd, LOCK_EX) == -1) {
        perror("Failed to obtain lock");
        exit(1);
    }
    
    
    
    // Write data to the file
    write(fd, "Data from C Executable", 22);
    sleep(20);
    
    // Release the lock and close the file
    flock(fd, LOCK_UN);
    close(fd);
    
    return 0;
}
