#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 25000000
#define RAND random() % MAX


#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_mutex_t mutex;
pthread_mutex_t s[N];

int state[N];

void thinking(int think) {
    long wait = RAND;
    printf("Philosopher %d thinking on %ld...\n", think, wait);
    for(long i = 0; i < wait; i++);

    printf("Philosopher %d finish thinking\n", think);
}

int p = 1;
long count[N];
void eating(int eat) {
    long wait = RAND;
    printf("Philosopher %d eating on %ld...\n", eat, wait);
    for(long i = 0; i < wait; i++);
    printf("Philosopher %d finish eating [%ld times]\n", eat, ++count[eat]);

    int f = 1;
    for(int i = 0; i < N && f; i++) {
        f &= count[i] >= p;
    }
    if(f) {
        printf("[all counters >= %d]\n", p);
        p *= 2;
    }
}

void test(int i) {
    if(state[i] == HUNGRY && state[(i + 1) % N] != EATING && state[(i + N - 1) % N] != EATING) {
        pthread_mutex_unlock(s + i);
        state[i] = EATING;
    }
}

_Noreturn void* philosopher(void* arg) {
    int i = (int) arg;
    printf("philosopher created %d\n", i);

    int think = i;
    int eat = i;


    while(1) {
        thinking(think);

        pthread_mutex_lock(&mutex);
        state[i] = HUNGRY;
        test(i);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(s+i);
        eating(eat);

        pthread_mutex_lock(&mutex);
        state[i] = THINKING;
        test((i + 1) % N);
        test((i + N - 1) % N);
        pthread_mutex_unlock(&mutex);
    }
}


int main() {
    printf("Hello, World!\n");

    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < N; i++) {
        count[i] = 0;
        state[i] = THINKING;
        pthread_mutex_init(s+i, NULL);
        pthread_mutex_lock(s+i);
    }


    pthread_t pid[N];

    for(void* i = 0; (int) i < N; i++) {
        pthread_create(pid+(int)i, NULL, philosopher, i);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(pid[i], NULL);
    }

    return 0;
}
