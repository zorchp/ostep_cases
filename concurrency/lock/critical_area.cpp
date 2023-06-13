int LoadLinked(int *ptr) { return *ptr; }

int StoreConditional(int *ptr, int val) {
    if (/* 上一次加载的地址在某期间都没有更新 */ 1) {
        *ptr = val;
        return 1;
    } else
        return 0;
}
typedef struct {
    int flg;
} lock_t;

void lock(lock_t *lock) {
    while (1) {
        while (LoadLinked(&lock->flg) == 1)
            ;
        if (StoreConditional(&lock->flg, 1) == 1) return;
    }
}

void unlock(lock_t *lock) { //
    lock->flg = 0;
}
