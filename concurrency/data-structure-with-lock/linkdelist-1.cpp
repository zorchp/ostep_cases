#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <pthread.h>

typedef struct node_t {
    int key;
    struct node_t *next;
} node_t;
typedef struct list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

namespace basic_linkedlist {
void list_init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int list_insert(list_t *L, int key) {
    pthread_mutex_lock(&L->lock);
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc"); // malloc 如果失败返回空指针, 此时必须释放锁
        pthread_mutex_unlock(&L->lock);
        return -1;
    }
    new_node->key = key;
    new_node->next = L->head;
    L->head = new_node;
    pthread_mutex_unlock(&L->lock);
    return 0; // success
}

int list_lookup(list_t *L, int key) {
    pthread_mutex_lock(&L->lock);
    node_t *cur = L->head;
    while (cur) {
        if (cur->key == key) {
            pthread_mutex_unlock(&L->lock);
            return 0;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&L->lock);
    return -1; // failure
}
} // namespace basic_linkedlist


namespace improve_linkedlist {

void list_init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int list_insert(list_t *L, int key) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc"); // malloc 如果失败返回空指针, 此时必须释放锁
        return -1;
    }
    pthread_mutex_lock(&L->lock); // 仅在临界区加锁
    new_node->key = key;
    new_node->next = L->head;
    L->head = new_node;
    pthread_mutex_unlock(&L->lock);
    return 0;
}

int list_lookup(list_t *L, int key) {
    int rv = -1;
    pthread_mutex_lock(&L->lock);
    node_t *cur = L->head;
    while (cur) {
        if (cur->key == key) {
            rv = 0;
            break;
        }
        cur = cur->next;
    }
    pthread_mutex_unlock(&L->lock);
    return rv; // both success and failure
}

list_t *ll = new list_t;
void *f_insert(void *) {
    for (int i{}; i < 100; ++i) { //
        list_insert(ll, i);
    }
    return NULL;
}

} // namespace improve_linkedlist

void t1() {
    using namespace improve_linkedlist;
    list_init(ll);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, f_insert, NULL);
    pthread_create(&tid2, NULL, f_insert, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    for (auto cur = ll->head; cur; cur = cur->next) //
        printf("%d\t", cur->key);
}

int main(int argc, char *argv[]) {
    t1();
    return 0;
}
