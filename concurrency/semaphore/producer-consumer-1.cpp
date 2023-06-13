#include <pthread.h>
#include <cstdio>
#include <unistd.h>

#ifdef __APPLE__
#include "zemaphore.h"
#elif linux
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


void t1() {
    using namespace version1;
    sem_init(&empty, MAX);
    sem_init(&full, 0);
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
