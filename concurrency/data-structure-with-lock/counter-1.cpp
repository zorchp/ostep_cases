#include <cstdio>
#include <pthread.h>

namespace not_safe {
typedef struct counter_t {
    int val;
} counter_t;

void init(counter_t* c) { c->val = 0; }
void inc(counter_t* c) { c->val++; }
void dec(counter_t* c) { c->val--; }
int get(counter_t* c) { return c->val; }
} // namespace not_safe

namespace safe {
typedef struct counter_t {
    int val;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t* c) {
    c->val = 0;
    pthread_mutex_init(&c->lock, NULL);
}
void inc(counter_t* c) {
    pthread_mutex_lock(&c->lock);
    c->val++;
    pthread_mutex_unlock(&c->lock);
}
void dec(counter_t* c) {
    pthread_mutex_lock(&c->lock);
    c->val--;
    pthread_mutex_unlock(&c->lock);
}
int get(counter_t* c) {
    pthread_mutex_lock(&c->lock);
    int tmp = c->val;
    pthread_mutex_unlock(&c->lock);
    return tmp;
}

} // namespace safe

namespace lazy {
const int NUMCPUS = 8;

typedef struct counter_t {
    int global; // 全局计数器
    pthread_mutex_t glock;
    int local[NUMCPUS];
    pthread_mutex_t llock[NUMCPUS];
    int threshold;
} counter_t;

void init(counter_t* c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);
    for (int i = 0; i < NUMCPUS; ++i) {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}
void update(counter_t* c, int threadID, int amt) {
    pthread_mutex_lock(&c->llock[threadID]);
    c->local[threadID] += amt;
    if (c->local[threadID] >= c->threshold) {
        pthread_mutex_lock(&c->glock);
        c->global += c->local[threadID];
        pthread_mutex_unlock(&c->glock);
        c->local[threadID] = 0;
    }
    pthread_mutex_unlock(&c->llock[threadID]);
}
int get(counter_t* c) {
    pthread_mutex_lock(&c->glock);
    int tmp = c->global;
    pthread_mutex_unlock(&c->glock);
    return tmp;
}

} // namespace lazy

using namespace lazy;
counter_t cnt;
void* f(void*) {
    for (int i{}; i < 10000000; ++i) //
        update(&cnt, 1, 1);

    return NULL;
}
void t1() {
    init(&cnt, 1000);
    pthread_t tids[NUMCPUS];
    for (int i = 0; i < NUMCPUS; ++i) {
        pthread_create(&tids[i], NULL, f, NULL);
        pthread_join(tids[i], NULL);
    }
    printf("%d\n", get(&cnt));
    // 80000000
}

int main(int argc, char* argv[]) {
    t1();
    return 0;
}
