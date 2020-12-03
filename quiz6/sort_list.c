#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct ListNode {
    int val;
    struct ListNode *next;
};

void insert(struct ListNode **l, int d)
{
    struct ListNode *tmp = malloc(sizeof(struct ListNode));
    tmp->val = d;

    if (!(*l))
        tmp->next = NULL;
    else {
        tmp->next = *l;
    }
    *l = tmp;
}

struct ListNode *SortedMerge(struct ListNode *a, struct ListNode *b)
{
    struct ListNode dummy;
    struct ListNode *tail = &dummy;

    dummy.next = NULL;

    while (1) {
        if (a == NULL) {
            tail->next = b;
            break;
        } else if (b == NULL) {
            tail->next = a;
            break;
        }

        if (a->val > b->val) {
            tail->next = b;
            b = b->next;
        } else {
            tail->next = a;
            a = a->next;
        }
        tail = tail->next;
    }
    return dummy.next;
}

void FrontBackSplit(struct ListNode *src,
                    struct ListNode **front,
                    struct ListNode **back)
{
    if (src == NULL || src->next == NULL) {
        *front = src;
        *back = NULL;
        return;
    }

    struct ListNode *slow = src;
    struct ListNode *fast = src->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = src;
    *back = slow->next;
    slow->next = NULL;
}


void sorted_insert(struct ListNode **head, struct ListNode *node)
{
    if (!*head || (*head)->val >= node->val) {
        node->next = *head;
        *head = node;
        return;
    }
    struct ListNode *current = *head;
    while (current->next && current->next->val < node->val)
        current = current->next;
    node->next = current->next;
    current->next = node;
}

struct ListNode *insertion_sort(struct ListNode *head)
{
    struct ListNode *sorted = NULL;
    for (struct ListNode *cur = head; cur;) {
        struct ListNode *next = cur->next;
        sorted_insert(&sorted, cur);
        cur = next;
    }

    return sorted;
}

unsigned int THR = 128;
struct ListNode *tiled_sortList(struct ListNode *head, unsigned int cnt)
{
    if (head == NULL || (head)->next == NULL)
        return head;

    struct ListNode *a;
    struct ListNode *b;

    FrontBackSplit(head, &a, &b);

    if ((cnt >> 1) < THR) {
        a = insertion_sort(a);
        b = insertion_sort(b);
    } else {
        a = tiled_sortList(a, (cnt >> 1) + (cnt & 1));
        b = tiled_sortList(b, (cnt >> 1));
    }

    return SortedMerge(a, b);
}

struct ListNode *sortList(struct ListNode *head)
{
    int cnt = 0;

    struct ListNode *tmp = head;
    while (head != NULL) {
        cnt++;
        head = head->next;
    }

    return tiled_sortList(tmp, cnt);
}

void dump(struct ListNode *head)
{
    while (head != NULL) {
        printf("%d ", head->val);
        head = head->next;
    }
    puts("");
}

void free_list(struct ListNode **l)
{
    while (*l) {
        struct ListNode *next = (*l)->next;
        free(*l);
        *l = next;
    }
}

int main()
{
    for (int NODE = 1; NODE < 5000; NODE += 100) {
        struct ListNode *head = NULL;
        srand(NODE);
        for (int i = 0; i < NODE; i++) {
            insert(&head, rand() % 10000);
        }

        struct timespec tt1, tt2;
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        THR = 64;
        head = sortList(head);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        free_list(&head);

        long long time1 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

        head = NULL;
        srand(NODE);
        for (int i = 0; i < NODE; i++) {
            insert(&head, rand() % 10000);
        }

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        THR = 0;
        head = sortList(head);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        free_list(&head);

        long long time2 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

        head = NULL;
        srand(NODE);
        for (int i = 0; i < NODE; i++) {
            insert(&head, rand() % 10000);
        }
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        THR = 0;
        head = insertion_sort(head);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        free_list(&head);

        long long time3 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

        printf("%d, %f, %f, %f\n", NODE, time3 / 1e6, time2 / 1e6, time1 / 1e6);
    }

    /*
    for (THR = 0; THR < 500; THR++) {
        struct ListNode *head = NULL;
        srand(1);
    for (int i = 0; i < 5000; i++) {
            insert(&head, rand() % 10000);
        }
        // dump(head);

        struct timespec tt1, tt2;
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        head = sortList(head);
        clock_gettime(CLOCK_MONOTONIC, &tt2);

        long long time = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                         (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);


        // dump(head);
        free_list(&head);
        printf("%d, %f\n", THR, time / 1e6);
    }
    */
    return 0;
}
