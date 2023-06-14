#ifndef __zemaphore_h__
#define __zemaphore_h__
#include <pthread.h>

typedef struct __Zem_t {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

void Zem_init(Zem_t *z, int value) {
    z->value = value;
    pthread_cond_init(&z->cond, NULL);
    pthread_mutex_init(&z->lock, NULL);
}

void Zem_wait(Zem_t *z) {
    // 如果 val >= 1, 立即返回
    // 否则线程挂起, 直到之后的 post 操作
    pthread_mutex_lock(&z->lock);
    while (z->value <= 0) //
        pthread_cond_wait(&z->cond, &z->lock);
    --z->value;
    pthread_mutex_unlock(&z->lock);
}

void Zem_post(Zem_t *z) {
    // 并不等待某些条件满足, 仅增加信号量的值, 如果有等待线程, 唤醒其中一个
    pthread_mutex_lock(&z->lock);
    ++z->value;
    pthread_cond_signal(&z->cond); // 唤醒
    pthread_mutex_unlock(&z->lock);
}

#ifdef __APPLE__ // just for APPLE
typedef Zem_t sem_t;

#define sem_wait(s) Zem_wait(s)
#define sem_post(s) Zem_post(s)
// placeholder for API compatibliity
#define sem_init(s, placeholder, v) Zem_init(s, v)
#endif

#endif // __zemaphore_h__
