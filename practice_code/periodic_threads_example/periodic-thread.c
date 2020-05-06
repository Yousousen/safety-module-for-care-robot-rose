#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define NSEC_PER_SEC 1000000000ULL
/* HACK, needed because of a bug in old glibc */
/* extern int clock_nanosleep(clockid_t __clock_id, int __flags, */
/*                            __const struct timespec *__req, */
/*                            struct timespec *__rem); */




struct periodic_task;
struct periodic_task *start_periodic_timer(unsigned long long offs, int t);
void wait_next_activation(struct periodic_task *t);


void task1(void)
{
  int i,j;
 
  for (i=0; i<3; i++) {
    for (j=0; j<1000; j++) ;
    printf("1");
    fflush(stdout);
  }
}

void task2(void)
{
  int i,j;

  for (i=0; i<5; i++) {
    for (j=0; j<10000; j++) ;
    printf("2");
    fflush(stdout);
  }
}

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


struct th_info {
  int period;
  void (*body)(void);
};

static void *thread_body(void *p)
{
    struct periodic_task *t;
    struct th_info *t_i = p;

    t = start_periodic_timer(2000000, t_i->period);
    if (t == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(t);
        t_i->body();
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
    t_3.period = 70000;
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
    sleep(20);

    kill_threads(id);

    return 0;
}

struct periodic_task {
  struct timespec r;
  int period;
};

static inline void timespec_add_us(struct timespec *t, unsigned long long d)
{
    d *= 1000;
    d += t->tv_nsec;
    while (d >= NSEC_PER_SEC) {
        d -= NSEC_PER_SEC;
	t->tv_sec += 1;
    }
    t->tv_nsec = d;
}

void wait_next_activation(struct periodic_task *t)
{
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t->r, NULL);
    timespec_add_us(&t->r, t->period);
}

struct periodic_task *start_periodic_timer(unsigned long long offs, int t)
{
    struct periodic_task *p;

    p = malloc(sizeof(struct periodic_task));
    if (p == NULL) {
        return NULL;
    }

    clock_gettime(CLOCK_REALTIME, &p->r);
    timespec_add_us(&p->r, offs);
    p->period = t;

    return p;
}


