#include <cstdio>
#include <pthread.h>
#include <unistd.h>

namespace spin {

volatile int done = 0;
void *child(void *arg) {
    printf("child\n");
    //
    done = 1; // ok but too slow
    return NULL;
}
} // namespace spin

void t1() {
    using namespace spin;
    printf("parent begin \n");
    pthread_t tid;
    pthread_create(&tid, NULL, child, NULL);
    while (done == 0)
        ; // spin
    printf("parent over \n");
    // parent begin
    // child
    // parent over
}

namespace cond_variable {
// pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m);
// pthread_cond_signal(pthread_cond_t *c);
int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() {
    pthread_mutex_lock(&m);
    done = 1;
    pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    sleep(1);
    thr_exit();
    return NULL;
}

void thr_join() {
    pthread_mutex_lock(&m);
    // if (done == 0) // also ok
    while (done == 0) //
        pthread_cond_wait(&c, &m);
    pthread_mutex_unlock(&m);
}
} // namespace cond_variable

void t2() {
    using namespace cond_variable;

    printf("parent begin \n");
    pthread_t tid;
    pthread_create(&tid, NULL, child, NULL);
    thr_join();
    printf("parent over \n");
    // parent begin
    // child
    // parent over
}

int main(int argc, char *argv[]) {
    // t1();
    t2();
    return 0;
}
