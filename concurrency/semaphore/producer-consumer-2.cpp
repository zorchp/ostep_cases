#ifdef __APPLE__
#include "zemaphore.h"
#elif __linux__
#include <semaphore.h>
#endif
#include <pthread.h>
#include <unistd.h>
#include <cstdio>

const int NUM = 3; // 括号嵌套数量
const int THREADS = 4;
sem_t empty, fill;

void *producer(void *) {
    while (1) {
        sem_wait(&empty);
        putchar('(');
        usleep(500);
        sem_post(&fill);
    }
    return NULL;
}

void *consumer(void *) {
    while (1) {
        sem_wait(&fill);
        putchar(')');
        usleep(500);
        sem_post(&empty);
    }
    return NULL;
}
int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); // 禁用缓冲区
    sem_init(&fill, 0, 0);
    sem_init(&empty, 0, NUM);
    pthread_t tids[THREADS];
    for (int i = 0; i < THREADS; i += 2) {
        pthread_create(&tids[i], NULL, producer, NULL);
        pthread_create(&tids[i + 1], NULL, consumer, NULL);
    }
    for (int i = 0; i < THREADS; ++i) {
        pthread_join(tids[i], NULL);
    }

    return 0;
}
