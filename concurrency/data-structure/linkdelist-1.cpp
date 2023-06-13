#include <cstdio>
#include <cstdlib>
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
} // namespace improve_linkedlist
