#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

// Global resource accessible to all threads
int global_resource;

// Threading routine which increments the resource 10 times and prints
// it after every increment
void* thread_inc (void* arg)
{
    for (int i = 0; i < 10; i++)
    {
        global_resource++;
        printf("Increment: %d\n", global_resource);
        // Make this thread slower, so the other one
        // can do more work
        sleep(1);
    }

    printf("Thread inc finished.\n");

    return NULL;
}

// Threading routine which decrements the resource 10 times and prints
// it after every decrement
void* thread_dec (void* arg)
{
    for (int i = 0; i < 10; i++)
    {
        global_resource--;
        printf("Decrement: %d\n", global_resource);
    }

    printf("Thread dec finished.\n");

    return NULL;
}

int main (int argc, char** argv)
{
    pthread_t threads[2];

    if (0 != (errno = pthread_create(&threads[0], NULL, thread_inc, NULL)))
    {
        perror("pthread_create() failed");
        return EXIT_FAILURE;
    }

    if (0 != (errno = pthread_create(&threads[1], NULL, thread_dec, NULL)))
    {
        perror("pthread_create() failed");
        return EXIT_FAILURE;
    }

    // Wait for threads to finish
    for (int i = 0; i < 2; i++)
    {
        if (0 != (errno = pthread_join(threads[i], NULL))) {
            perror("pthread_join() failed");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
