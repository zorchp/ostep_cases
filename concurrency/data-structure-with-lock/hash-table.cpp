#include <pthread.h>
#include <cstdio>
#include <cstdlib>


const int BUCKETS = 101; // 桶大小

typedef struct node_t {
    int key;
    struct node_t *next;
} node_t;
typedef struct list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;
void list_init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int list_insert(list_t *L, int key) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc"); // malloc 如果失败返回空指针, 此时必须释放锁
        return -1;        // failure
    }
    pthread_mutex_lock(&L->lock); // 仅在临界区加锁
    new_node->key = key;
    new_node->next = L->head;
    L->head = new_node;
    pthread_mutex_unlock(&L->lock);
    return 0; // success
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

typedef struct __hash_t {
    list_t lists[BUCKETS]; // 拉链法
} hash_t;

void hash_init(hash_t *h) {
    for (int i = 0; i < BUCKETS; ++i) {
        list_init(&h->lists[i]);
    }
}

int hash_insert(hash_t *h, int key) {
    int bucket = key % BUCKETS;
    return list_insert(&h->lists[bucket], key);
}

int hash_lookup(hash_t *h, int key) {
    int bucket = key % BUCKETS;
    return list_lookup(&h->lists[bucket], key);
}
