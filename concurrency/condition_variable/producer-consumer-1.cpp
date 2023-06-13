#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <cstdio>

namespace version1_2_3 {
int buf; // 缓冲区
int cnt; // 数据
const int LOOP_NUMS = 5;
void put(int val) {
    assert(cnt == 0);
    cnt = 1;
    buf = val;
}
int get() {
    assert(cnt == 1);
    cnt = 0;
    return buf;
}

pthread_cond_t empty;
pthread_cond_t full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int loops = LOOP_NUMS;
    for (int i = 0; i < loops; ++i) {
        pthread_mutex_lock(&mutex);
        while (cnt == 1) // 满了, 等待变空
            pthread_cond_wait(&empty, &mutex);
        put(i);
        printf("%d put \n", i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int loops = LOOP_NUMS;
    for (int i = 0; i < loops; ++i) {
        pthread_mutex_lock(&mutex);
        while (cnt == 0) // 空了, 等待变满
            pthread_cond_wait(&full, &mutex);
        int tmp = get();
        printf("%d gotten \n", tmp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
} // namespace version1_2_3

void t1() {
    using namespace version1_2_3;
    // version 2: 将 cond_wait 的 if 改为 while, 但是会导致三个线程都睡眠
    // 原因在于仅有一个条件变量, 使得消费者唤醒了消费者
    // 使用第二个条件变量解决这个问题
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);
    pthread_create(&tid3, NULL, consumer, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
}

namespace version_final {
//
const int MAX = 5;
const int LOOP_NUMS = 10;
int buf[MAX];
int fill = 0;
int use = 0;
int cnt = 0;

void put(int val) {
    buf[fill] = val;
    fill = (fill + 1) % MAX;
    ++cnt;
}

int get() {
    int tmp = buf[use];
    use = (use + 1) % MAX;
    --cnt;
    return tmp;
}
pthread_cond_t empty;
pthread_cond_t full;
pthread_mutex_t mutex;

void *producer(void *arg) {
    int loops = LOOP_NUMS;
    for (int i = 0; i < loops; ++i) {
        pthread_mutex_lock(&mutex);
        while (cnt == MAX) // 满了, 等待变空
            pthread_cond_wait(&empty, &mutex);
        put(i);
        printf("%d put \n", i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int loops = LOOP_NUMS;
    for (int i = 0; i < loops; ++i) {
        pthread_mutex_lock(&mutex);
        while (cnt == 0) // 空了, 等待变满
            pthread_cond_wait(&full, &mutex);
        int tmp = get();
        usleep(30000);
        printf("%d gotten \n", tmp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

} // namespace version_final

void t2() {
    using namespace version_final;
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);
    // pthread_create(&tid3, NULL, consumer, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // pthread_join(tid3, NULL);
    // 1生产者2 消费者还是会忙等
}

int main(int argc, char *argv[]) {
    // t1();
    t2();
    return 0;
}
