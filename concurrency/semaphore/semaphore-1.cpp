#include <semaphore.h>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>
namespace sem_as_lock {
sem_t s;
int cnt = 0;
void *f(void *) {
    sem_wait(&s);
    for (int i = 0; i < 1000000; ++i) { //
        cnt = cnt + 1;
    }
    sem_post(&s);
    return NULL;
}
} // namespace sem_as_lock

void t1() {
    using namespace sem_as_lock;
    sem_init(&s, 0, 1);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, f, NULL);
    pthread_create(&tid2, NULL, f, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // sleep(1);
    printf("cnt=%d\n", cnt);
}

namespace sem_as_cond_variable {
sem_t s;
void *child(void *arg) {
    printf("child\n");
    sem_post(&s); // signal : child done
    return NULL;
}

} // namespace sem_as_cond_variable

void t2() {
    using namespace sem_as_cond_variable;
    sem_init(&s, 0, 0);
    printf("parent: begin\n");
    pthread_t tid;
    pthread_create(&tid, NULL, child, NULL);
    sem_wait(&s);
    printf("parent: end\n");
    // test in ubuntu: x86
    // parent: begin
    // child
    // parent: end
}

int main(int argc, char *argv[]) {
    t1();
    // t2();
    return 0;
}
