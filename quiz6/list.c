#include <stdio.h>

/* clang-format off */
#define cons(x, y) (struct llist[]){{y, x}}
/* clang-format on */

struct llist {
    int val;
    struct llist *next;
};

void sorted_insert(struct llist **head, struct llist *node)
{
    if (!*head || (*head)->val >= node->val) {
        node->next = *head;
        *head = node;
        return;
    }
    struct llist *current = *head;
    while (current->next && current->next->val < node->val)
        current = current->next;
    node->next = current->next;
    current->next = node;
}

void sort(struct llist **head)
{
    struct llist *sorted = NULL;
    for (struct llist *current = *head; current;) {
        struct llist *next = current->next;
        sorted_insert(&sorted, current);
        current = next;
    }
    *head = sorted;
}

int main()
{
    struct llist *list = cons(cons(cons(cons(NULL, 7), 4), 5), 9);
    struct llist *p;
    for (p = list; p; p = p->next)
        printf("%d", p->val);
    printf("\n");

    sort(&list);
    for (p = list; p; p = p->next)
        printf("%d", p->val);
    printf("\n");
    return 0;
}
