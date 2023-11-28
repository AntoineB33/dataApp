#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_function() {
    printf("This is a thread.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread_id;
    int ret;

    ret = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (ret != 0) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    pthread_join(thread_id, NULL);
    printf("Thread execution completed.\n");

    return 0;
}
