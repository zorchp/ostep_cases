#include <pthread.h>
#include <cstdio>
#include <unistd.h>

#ifdef __APPLE__
#include "zemaphore.h"
#elif __linux__
#include <semaphore.h>
#endif
namespace version1 {
const int MAX = 10;
const int LOOP_NUMS = 100;
int buf[MAX];
int fill = 0, use = 0;

void put(int val) {
    buf[fill] = val;
    fill = (fill + 1) % MAX;
}

int get() {
    int tmp = buf[use];
    use = (use + 11) % MAX;
    return tmp;
}

sem_t empty, full;
void* producer(void* arg) {
    for (int i = 0; i < LOOP_NUMS; ++i) {
        sem_wait(&empty);
        put(i);
        usleep(5000);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    int tmp = 0;
    while (-1 != tmp) {
        sem_wait(&full);
        tmp = get();
        usleep(5000);
        sem_post(&empty);
        printf("%d\n", tmp);
    }
    return NULL;
}
} // namespace version1


namespace version2 { // 死锁
const int MAX = 10;
const int LOOP_NUMS = 100;
int buf[MAX];
int fill = 0, use = 0;

void put(int val) {
    buf[fill] = val;
    fill = (fill + 1) % MAX;
}

int get() {
    int tmp = buf[use];
    use = (use + 11) % MAX;
    return tmp;
}

sem_t empty, full;
sem_t mutex;
void* producer(void* arg) {
    for (int i = 0; i < LOOP_NUMS; ++i) {
        sem_wait(&mutex);
        sem_wait(&empty);
        put(i);
        usleep(5000);
        sem_post(&full);
        sem_post(&mutex);
    }
    return NULL;
}

void* consumer(void* arg) {
    int tmp = 0;
    while (-1 != tmp) {
        sem_wait(&mutex);
        sem_wait(&full);
        tmp = get();
        usleep(5000);
        sem_post(&empty);
        sem_post(&mutex);
        printf("%d\n", tmp);
    }
    return NULL;
}
} // namespace version2

namespace version3 { // ok
const int MAX = 10;
const int LOOP_NUMS = 100;
int buf[MAX];
int fill = 0, use = 0;

void put(int val) {
    buf[fill] = val;
    fill = (fill + 1) % MAX;
}

int get() {
    int tmp = buf[use];
    use = (use + 11) % MAX;
    return tmp;
}

sem_t empty, full;
sem_t mutex;
void* producer(void* arg) {
    for (int i = 0; i < LOOP_NUMS; ++i) {
        sem_wait(&empty);
        sem_wait(&mutex);
        put(i);
        usleep(5000);
        sem_post(&mutex);
        sem_post(&full);
    }
    // end case
    for (int i = 0; i < 2; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);
        put(-1);
        sem_post(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    int tmp = 0;
    while (-1 != tmp) {
        sem_wait(&full);
        sem_wait(&mutex);
        tmp = get();
        usleep(5000);
        sem_post(&mutex);
        sem_post(&empty);
        printf("%d\n", tmp);
    }
    return NULL;
}
} // namespace version3


void t1() {
    // using namespace version1;
    // using namespace version2;
    using namespace version3;
    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);
    pthread_create(&tid3, NULL, consumer, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
}

int main(int argc, char* argv[]) {
    t1();
    return 0;
}
