#include <cassert>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>


typedef struct __node_t {
    int val;
    struct __node_t* next;
} node_t;


typedef struct queue_t {
    node_t *head, *tail;
    pthread_mutex_t headLock, tailLock;
} queue_t;

void queue_init(queue_t* q) {
    node_t* tmp = (node_t*)malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp; // 中节点, 分割实际入队和出队的序列
    pthread_mutex_init(&q->headLock, NULL);
    pthread_mutex_init(&q->tailLock, NULL);
}

void enqueue(queue_t* q, int val) {
    node_t* dummy = (node_t*)malloc(sizeof(node_t));
    assert(dummy != NULL);
    dummy->val = val;
    dummy->next = NULL;

    pthread_mutex_lock(&q->tailLock); // 队尾插入
    q->tail->next = dummy;
    q->tail = dummy;
    pthread_mutex_unlock(&q->tailLock); // 队尾插入
}

int dequeue(queue_t* q, int* val) {
    pthread_mutex_lock(&q->headLock); // 队头删除
    node_t* tmp = q->head;
    node_t* newHead = tmp->next;
    if (newHead == NULL) {
        pthread_mutex_unlock(&q->headLock);
        free(tmp); //
        return -1; // queue empty
    }
    *val = newHead->val;
    q->head = newHead;
    pthread_mutex_unlock(&q->headLock);
    free(tmp);
    return 0;
}

void t1() { // basic test
    queue_t* q = (queue_t*)malloc(sizeof(queue_t));
    queue_init(q);
    for (int i{}; i < 10; ++i) { //
        enqueue(q, i);
    }
    for (int i{}; i < 5; ++i) { //
        int tmp;
        dequeue(q, &tmp);
        printf("%d\t", tmp);
    }
    // 0       1       2       3       4
    putchar('\n');
    for (int i{}; i < 10; ++i) { //
        enqueue(q, i);
    }
    for (int i{}; i < 10; ++i) { //
        int tmp;
        dequeue(q, &tmp);
        printf("%d\t", tmp);
    }
    putchar('\n');
    // 5       6       7       8       9       0       1       2       3       4
}

int main(int argc, char* argv[]) {
    t1();
    return 0;
}
