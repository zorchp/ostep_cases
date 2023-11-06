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
    for (int i = 0; i < LOOP_NUMS; ++i) {
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
    for (int i = 0; i < LOOP_NUMS; ++i) {
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
const int producer_num = 1;
const int consumer_num = 5;

const int MAX = 4;
const int LOOP_NUMS = 1;

int buf[MAX]{};
int fill = 0;
int use = 0;
int cnt = 0;

void put(int val) {
    buf[fill] = val;
    fill = (fill + 1) % MAX; // 循环向缓冲区中加入元素
    ++cnt;
}

int get() {
    int tmp = buf[use];
    use = (use + 1) % MAX; // 循环从缓冲区中取出元素
    --cnt;
    return tmp;
}
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    for (int i = 0; i < LOOP_NUMS; ++i) {
        pthread_mutex_lock(&mutex);
        while (cnt == MAX) // 满了, 等待变空
            pthread_cond_wait(&empty, &mutex);
        put(i);
        printf("%d put producer \n", i);
        pthread_cond_signal(&full); // 发送满了的信号
        pthread_mutex_unlock(&mutex);
    }
    for (int i = 0; i < consumer_num; ++i) { // 消费者数量
        pthread_mutex_lock(&mutex);
        while (cnt == MAX) // 满了, 等待变空
            pthread_cond_wait(&empty, &mutex);
        // 通过锁来实现这种同步更普适, 因为用-1
        // 标记的方式没办法处理多生产者的情况.
        put(-1); // 标记消费者, 消费的对应的生产出的,
                 // 一一对应使得消费者之间不会互相争抢.
        // 如果生产出的比较少, 此时也执行标记,
        // (条件变量不会等待, 程序继续走到放置-1 位置)
        // 这样不会出现消费者一直忙等生产者生产出数据的情况, 使程序正常退出.
        printf("%d put -1 for consumer \n", i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int tmp = 0;
    while (tmp != -1) { // 遇到了-1, 消费者退出
        pthread_mutex_lock(&mutex);
        while (cnt == 0) // 空了, 等待变满
            pthread_cond_wait(&full, &mutex);
        tmp = get(); //-1 设置为数据边界
        // usleep(3'000); // 3ms
        printf("%d gotten \n", tmp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

} // namespace version_final


void t2() {
    using namespace version_final;
    pthread_t ps[producer_num], cs[consumer_num];
    for (int i{}; i < producer_num; ++i) {
        pthread_create(&ps[i], NULL, producer, NULL);
    }
    for (int i{}; i < consumer_num; ++i) {
        pthread_create(&cs[i], NULL, consumer, NULL);
    }
    // 必须先创建再 join, 否则生产者会忙等
    for (int i{}; i < producer_num; ++i) {
        pthread_join(ps[i], NULL);
    }
    for (int i{}; i < consumer_num; ++i) {
        pthread_join(cs[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    // t1();
    t2();
    return 0;
}
