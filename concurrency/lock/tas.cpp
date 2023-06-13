#include <iostream>
#include <thread>
using namespace std;

int TestAndSet(int* old_ptr, int new_val) {
    int old = *old_ptr; // 取出旧值
    *old_ptr = new_val; // 设置新值
    return old;         // 返回旧值
}

typedef struct lock_t {
    int flg;
} lock_t;

// 初始时为 0 表示未持有, 1 表示持有
void init(lock_t* lock) { lock->flg = 0; }

void lock(lock_t* lock) {
    // 持有, 一直等待
    while (TestAndSet(&lock->flg, 1) == 1) // TEST flg
        ;                                  // spin wait
    lock->flg = 1;                         // SET
}

void unlock(lock_t* lock) { lock->flg = 0; }
int i{};

lock_t lk;
void f() {
    int num = 100000;
    lock(&lk);
    for (int n{}; n < num; ++n) {
        i = i + 1;
    }
    unlock(&lk);
}

int main(int argc, char* argv[]) {
    init(&lk);
    thread t1(f);
    thread t2(f);
    t1.join();
    t2.join();
    cout << i << endl; // 10719

    return 0;
}
