#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_MAX_HEIGHT 32

typedef unsigned long custom_t; /* specify user-defined type */

typedef struct __list_node {
    uint8_t *key;
    size_t key_len;
    uint8_t height;
    custom_t val;
    struct __list_node *next[1];
} list_node_t;

typedef struct {
    uint8_t height;
    uint32_t bits, reset;
    list_node_t *head;
} skiplist;

#define LIST_ALLOC(e, s)               \
    do {                               \
        e = malloc(s);                 \
        assert(e && "out of memory."); \
        memset(e, 0, s);               \
    } while (0)

#define LIST_HEIGHT(d, str, l, z)                                  \
    do {                                                           \
        bool flip = false;                                         \
        for (z = 0; !flip; z++) {                                  \
            if (!d->reset) {                                       \
                d->bits = 5381; /* djb2 hash */                    \
                for (size_t i = 0; i < l; i++)                     \
                    d->bits = ((d->bits << 5) + d->bits) + str[i]; \
                d->reset = LIST_MAX_HEIGHT;                        \
            }                                                      \
            flip = (bool) d->bits & 1;                             \
            d->bits >>= 1;                                         \
            --(d->reset);                                          \
        }                                                          \
    } while (0)

#define LIST_INIT(d)                                                          \
    do {                                                                      \
        LIST_ALLOC(d, sizeof(skiplist));                                      \
        LIST_ALLOC(d->head, (sizeof(list_node_t) + ((sizeof(list_node_t *)) * \
                                                    (LIST_MAX_HEIGHT - 1)))); \
        d->height = 0;                                                        \
    } while (0)

#define LIST_GET(d, k, l, v) LIST_FIND(d, k, l, v, NULL)

#define LIST_FIND(d, k, l, v, u)                                          \
    do {                                                                  \
        list_node_t *iterator = d->head, **ud = u;                        \
        for (int i = d->height - 1; i >= 0; --i) {                        \
            while (iterator->next[i] &&                                   \
                   memcmp(k, iterator->next[i]->key, l) > 0)              \
                iterator = iterator->next[i];                             \
            if (ud)                                                       \
                ud[i] = iterator;                                         \
        }                                                                 \
        list_node_t *n = iterator->next[0];                               \
        v = (n && l == n->key_len && !memcmp(k, n->key, l)) ? n->val : 0; \
    } while (0)

#define LIST_NODE_INIT(n, k, l, v, h)                                        \
    do {                                                                     \
        LIST_ALLOC(                                                          \
            n, (sizeof(list_node_t) + ((sizeof(list_node_t *)) * (h - 1)))); \
        LIST_ALLOC(n->key, l + 1);                                           \
        memcpy(n->key, k, l);                                                \
        n->val = (custom_t) v, n->height = h, n->key_len = l;                \
    } while (0)

#define LIST_INSERT(d, k, l, v)          \
    do {                                 \
        custom_t vl;                     \
        list_node_t *u[LIST_MAX_HEIGHT]; \
        LIST_FIND(d, k, l, vl, u);       \
        if (vl)                          \
            break;                       \
        int h;                           \
        LIST_HEIGHT(d, k, l, h);         \
        if (h > d->height) {             \
            h = ++(d->height);           \
            u[h - 1] = d->head;          \
        }                                \
        list_node_t *n;                  \
        LIST_NODE_INIT(n, k, l, v, h);   \
        while (--h >= 0) {               \
            n->next[h] = u[h]->next[h];  \
            u[h]->next[h] = n;           \
        }                                \
    } while (0)

#define LIST_ITERATE(d, callback)         \
    do {                                  \
        list_node_t *iterator = d->head;  \
        while (iterator->next[0]) {       \
            iterator = iterator->next[0]; \
            callback(iterator);           \
        }                                 \
    } while (0)



static void print_key(list_node_t *n)
{
    puts((const char *) n->key);
}

int main()
{
    /* initialize the list */
    skiplist *d;
    LIST_INIT(d);

    /* create a key */
    char *key = malloc(6);
    memcpy(key, "hello", 5);
    key[5] = 0;

    /* create a value */
    char *val = malloc(6);
    memcpy(val, "world", 5);
    val[5] = 0;

    /* insert into the list */
    size_t l = 5;
    LIST_INSERT(d, key, l, (custom_t) val);

    /* iterate through the list and print */
    LIST_ITERATE(d, print_key);

    /* retrieve from a list */
    custom_t v;
    LIST_GET(d, key, l, v);
    printf("Hello, %s!\n", (char *) v);

    return 0;
}
