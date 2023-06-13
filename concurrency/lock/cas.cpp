#include <cstdio>
#include <pthread.h>

int TestAndSet(int* old_ptr, int new_val) {
    int old = *old_ptr; // 取出旧值
    *old_ptr = new_val; // 设置新值
    return old;         // 返回旧值
}

// 伪码
// int CompareAndSwap(int* ptr, int expected, int new_val) {
//     int actual = *ptr;
//     if (actual == expected) //
//         *ptr = new_val;
//     return actual;
// }

// just x86_64
char CompareAndSwap(int* ptr, int old_val, int new_val) {
    unsigned char ans;
    __asm__ __volatile__(
        "lock\n"
        "cmpxchgl %2, %1\n"
        "sete %0\n"
        : "=q"(ans), "=m"(*ptr)
        : "r"(new_val), "m"(*ptr), "a"(old_val)
        : "memory");
    return ans;
}

typedef struct lock_t {
    int flg;
} lock_t;

// 初始时为 0 表示未持有, 1 表示持有
void init(lock_t* lock) { lock->flg = 0; }

void lock(lock_t* lock) {
    // 持有, 一直等待
    // while (TestAndSet(&lock->flg, 1) == 1) // TEST flg
    while (CompareAndSwap(&lock->flg, 0, 1) == 1) // TEST flg
        ;                                         // spin wait
    lock->flg = 1;                                // SET
}

void unlock(lock_t* lock) { lock->flg = 0; }

int i{};

lock_t lk;
void* f(void*) {
    int num = 100000;
    for (int n{}; n < num; ++n) {
        lock(&lk);
        i = i + 1;
        unlock(&lk);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    init(&lk);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, f, NULL);
    pthread_create(&tid2, NULL, f, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("%d\n", i);

    return 0;
}
