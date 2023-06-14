#include <pthread.h>
#include <cassert>
#include <functional>
using namespace std;

enum { A = 1, B, C, D, E };
static struct rule {
    int from, ch, to;
} rules[] = {
    {A, 'H', B}, //
    {B, 'O', C}, //
    {C, 'H', A}, //
    {B, 'H', D}, //
    {D, 'O', A}, //
    {A, 'O', E}, //
    {E, 'H', C}, //
};
class H2O {
    int next(char ch) {
        for (int i = 0; i < sizeof(rules) / sizeof(rules[0]); ++i) {
            struct rule* rule = &rules[i];
            if (rule->from == cur && rule->ch == ch) {
                return rule->to;
            }
        }
        return 0;
    }

    int can_print(char ch) { return next(ch) != 0 && quota > 0; }

    void H2O_before(char ch) {
        pthread_mutex_lock(&lk);
        while (!can_print(ch)) //
            pthread_cond_wait(&cv, &lk);
        --quota;
        pthread_mutex_unlock(&lk);
    }

    void H2O_after(char ch) {
        pthread_mutex_lock(&lk);
        ++quota;
        cur = next(ch);
        assert(cur);
        pthread_cond_broadcast(&cv); // 通知所有线程
        pthread_mutex_unlock(&lk);
    }

    int cur, quota;
    pthread_mutex_t lk;
    pthread_cond_t cv;

public:
    H2O()
        : cur(A),
          quota(1),
          lk(PTHREAD_MUTEX_INITIALIZER),
          cv(PTHREAD_COND_INITIALIZER) {}

    void hydrogen(function<void()> releaseHydrogen) {
        H2O_before('H');
        releaseHydrogen();
        H2O_after('H');
    }

    void oxygen(function<void()> releaseOxygen) {
        H2O_before('O');
        releaseOxygen();
        H2O_after('O');
    }
};
