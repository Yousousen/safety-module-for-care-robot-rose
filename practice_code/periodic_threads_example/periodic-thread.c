#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define NSEC_PER_SEC 1000000000ULL

struct th_info {
  int period;
  void (*body)(void);
};

struct periodic_task {
  struct timespec ts;
  int period;
};
struct periodic_task;


// Start a periodic timer with an offset.
// the function returns a new periodic task object, with the period set to t.
struct periodic_task *start_periodic_timer(unsigned long long offset_in_us, int period);
void wait_next_activation(struct periodic_task *ptask);


void task1(void)
{
  int i;
  /* int j; */
 
  for (i=0; i<3; i++) {
    /* for (j=0; j<1000; j++) ; */
    printf("1");
    fflush(stdout);
  }
}

void task2(void)
{
  int i;
  /* int j; */

  for (i=0; i<5; i++) {
    /* for (j=0; j<10000; j++) ; */
    printf("2");
    fflush(stdout);
  }
}

// Ellapsed time in milliseconds.
void task3(void)
{
  static unsigned long long previous;
  unsigned long long t;
  struct timespec ts;

  if (previous == 0) {
    clock_gettime(CLOCK_REALTIME, &ts);
    previous = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000L;
  }

  clock_gettime(CLOCK_REALTIME, &ts);
  t = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000L;
  printf("\tT: %Lu\n", t - previous);
  previous = t;
}


static void *thread_body(void *arg) {
    struct periodic_task *ptask;
    struct th_info *the_thread = arg;

    /* ptask = start_periodic_timer(2000000, the_thread->period); */
    ptask = start_periodic_timer(0, the_thread->period);
    if (ptask == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(ptask);
        the_thread->body();
    }

    return NULL;
}

static pthread_t *create_threads(void)
{
    static struct th_info t_1, t_2, t_3;
    static pthread_t id[3];
    int err;

    t_1.body = task1;
    t_1.period = 20000;
    err = pthread_create(&id[0], NULL, thread_body, &t_1);
    if (err) {
        printf("PThread Create");
    }

    t_2.body = task2;
    t_2.period = 50000;
    err = pthread_create(&id[1], NULL, thread_body, &t_2);
    if (err) {
        printf("PThread Create");
    }

    t_3.body = task3;
    /* t_3.period = 70000; */
    /* t_3.period = 1E6-1000; */
    // 1s
    t_3.period = 1E6;
    err = pthread_create(&id[2], NULL, thread_body, &t_3);
    if (err) {
        printf("PThread Create");
    }

    return id;
}

static void kill_threads(pthread_t *id)
{
    int i;

    for (i = 0; i < 3; i++) {
        pthread_cancel(id[i]);
    }
    for (i = 0; i < 3; i++) {
        pthread_join(id[i], NULL);
    }

}

int main(int argc, char *argv[])
{
    pthread_t *id;

    id = create_threads();
    // Kill threads after 10 seconds.
    sleep(10);

    kill_threads(id);

    return 0;
}

// Add period microseconds to timespec ts.
static inline void timespec_add_us(struct timespec *ts, unsigned long long period)
{
    // (1µs) = 1000 * (1ns)
    period *= 1000;
    // Add number of nanoseconds already in ts to period.
    period += ts->tv_nsec;
    // To set tv_sec to the correct number we look at how many seconds of
    // nanoseconds there are in period.
    while (period >= NSEC_PER_SEC) {
        period -= NSEC_PER_SEC;
	ts->tv_sec += 1;
    }
    // The nanoseconds left.
    ts->tv_nsec = period;
}

void wait_next_activation(struct periodic_task *ptask)
{
    // Suspend the thread until the time value specified by &t->ts has elapsed.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ptask->ts, NULL);
    // Add another period to the time specification.
    timespec_add_us(&ptask->ts, ptask->period);
}

struct periodic_task *start_periodic_timer(unsigned long long offset_in_us, int period)
{
    struct periodic_task *ptask;

    ptask = malloc(sizeof(struct periodic_task));
    if (ptask == NULL) {
        return NULL;
    }

    clock_gettime(CLOCK_REALTIME, &ptask->ts);
    timespec_add_us(&ptask->ts, offset_in_us);
    ptask->period = period;

    return ptask;
}


