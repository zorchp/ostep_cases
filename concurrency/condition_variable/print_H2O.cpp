#include <pthread.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

enum { A = 1, B, C, D, E };
struct rule {
    int from, ch, to;
} rules[] = {
    {A, 'H', B}, //
    {B, 'O', C}, //
    {C, 'O', A}, //
    {B, 'H', D}, //
    {D, 'O', A}, //
    {A, 'O', E}, //
    {E, 'H', C}, //
};

int cur = A, quota = 1;

pthread_mutex_t lk = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int next(char ch) {
    for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); ++i) {
        //
        struct rule* rule = &rules[i];
        if (rule->from == cur && rule->ch == ch) {
            return rule->to;
        }
    }
    return 0;
}

static int can_print(char ch) { return next(ch) != 0 && quota > 0; }

void fish_before(char ch) {
    pthread_mutex_lock(&lk);
    while (!can_print(ch)) //
        pthread_cond_wait(&cv, &lk);
    --quota;
    pthread_mutex_unlock(&lk);
}

void fish_after(char ch) {
    pthread_mutex_lock(&lk);
    ++quota;
    cur = next(ch);
    assert(cur);
    pthread_cond_broadcast(&cv); // 通知所有线程
    pthread_mutex_unlock(&lk);
}

const char roles[] = ".HHHHOOOHHH";
int len = strlen(roles);

void* fish_thread(void* arg) {
    char role = roles[rand() % len];
    while (1) {
        fish_before(role);
        putchar(role);
        fish_after(role);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);
    int NUMS = 5; // 至少三个线程
    pthread_t tids[NUMS];
    for (int i = 0; i < NUMS; ++i) {
        pthread_create(&tids[i], NULL, fish_thread, NULL);
    }
    for (int i = 0; i < NUMS; ++i) {
        pthread_join(tids[i], NULL);
    }
    // pthread_join(tid2, NULL);
    return 0;
}
