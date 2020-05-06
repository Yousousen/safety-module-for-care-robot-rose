#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

// arguments for threads
struct threadargs {
    // framebuffer mutex.
    pthread_mutex_t* mutex;
};

// Global resource accessible to all threads
int global_resource;
// Mutex protecting the resource
pthread_mutex_t mutex;

// Threading routine which increments the resource 10 times and prints
// it after every increment
void* thread_inc (void* arg)
{
    // Pointer to mutex is passed as an argument
    struct threadargs *args = (struct threadargs *)arg;
    // Pointer to mutex is passed as an argument.
    pthread_mutex_t* mutex = args->mutex;

    // Execute the following code without interrupts, all the way to the
    // point B
    if (0 != (errno = pthread_mutex_lock(mutex)))
    {
        perror("pthread_mutex_lock failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++)
    {
        global_resource++;
        printf("Increment: %d\n", global_resource);
        // Make this thread slower, so the other one
        // can do more work
        /* sleep(1); */
    }

    printf("Thread inc finished.\n");

    // Point B:
    if (0 != (errno = pthread_mutex_unlock(mutex)))
    {
        perror("pthread_mutex_unlock failed");
        exit(EXIT_FAILURE);
    }

    return NULL;
}

// Threading routine which decrements the resource 10 times and prints
// it after every decrement
void* thread_dec (void* arg)
{
    // Pointer to mutex is passed as an argument
    struct threadargs *args = (struct threadargs *)arg;
    // Pointer to mutex is passed as an argument.
    pthread_mutex_t* mutex = args->mutex;

    if (0 != (errno = pthread_mutex_lock(mutex)))
    {
        perror("pthread_mutex_lock failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++)
    {
        global_resource--;
        printf("Decrement: %d\n", global_resource);
    }

    printf("Thread dec finished.\n");

    // Point B:
    if (0 != (errno = pthread_mutex_unlock(mutex)))
    {
        perror("pthread_mutex_unlock failed");
        exit(EXIT_FAILURE);
    }

    return NULL;
}

int main (int argc, char** argv)
{
    pthread_t threads[2];

    struct threadargs threadargs;
    pthread_mutex_t mutex;
    threadargs.mutex = &mutex;


    // Create a mutex with the default parameters
    if (0 != (errno = pthread_mutex_init(&mutex, NULL)))
    {
        perror("pthread_mutex_init() failed");
        return EXIT_FAILURE;
    }

    if (0 != (errno = pthread_create(&threads[0], NULL, thread_inc,
                    (void*) &threadargs)))
    {
        perror("pthread_create() failed");
        return EXIT_FAILURE;
    }

    if (0 != (errno = pthread_create(&threads[1], NULL, thread_dec,
                    (void*) &threadargs)))
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

    // Both threads are guaranteed to be finished here, so we can safely
    // destroy the mutex
    if (0 != (errno = pthread_mutex_destroy(&mutex)))
    {
        perror("pthread_mutex_destroy() failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
