#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int n;
    int **parent;
    int max_level;
} TreeAncestor;

TreeAncestor *C(int n, int *parent, int parentSize)
{
    TreeAncestor *obj = malloc(sizeof(TreeAncestor));
    obj->parent = malloc(n * sizeof(int *));
    obj->n = n;

    int max_level = 32 - __builtin_clz(n);
    for (int i = 0; i < n; i++) {
        obj->parent[i] = malloc(max_level * sizeof(int));

        obj->parent[i][0] = parent[i];
        for (int j = 1; j < max_level; j++)
            obj->parent[i][j] = -1;
    }

    for (int j = 1; j < max_level; j++) {
        int quit = 1;
        for (int i = 0; i < parentSize; i++) {
            obj->parent[i][j] = obj->parent[i][j - 1] == -1
                                    ? -1
                                    : obj->parent[obj->parent[i][j - 1]][j - 1];
            if (obj->parent[i][j] != -1)
                quit = 0;
        }
        if (quit)
            break;
    }
    obj->max_level = max_level;
    return obj;
}

int G(TreeAncestor *obj, int node, int k)
{
    int max_level = obj->max_level;
    for (int i = 0; i < max_level && node != -1; ++i)
        if (k & (1 << i))
            node = obj->parent[node][i];
    return node;
}

void F(TreeAncestor *obj)
{
    for (int i = 0; i < obj->n; i++)
        free(obj->parent[i]);
    free(obj->parent);
    free(obj);
}


TreeAncestor *treeAncestorCreate(int n, int *parent, int parentSize)
{
    TreeAncestor *obj = malloc(sizeof(TreeAncestor));
    int max_level = 32 - __builtin_clz(n);

    obj->parent = malloc(max_level * sizeof(int *));
    obj->n = n;
    obj->max_level = max_level;

    obj->parent[0] = malloc(n * sizeof(int));
    for (int j = 0; j < n; j++)
        obj->parent[0][j] = parent[j];

    for (int i = 1; i < max_level; i++) {
        obj->parent[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
            obj->parent[i][j] = -1;
    }



    for (int i = 1; i < max_level; i++) {
        int quit = 1;
        for (int j = 0; j < parentSize; j++) {
            obj->parent[i][j] = obj->parent[i - 1][j] == -1
                                    ? -1
                                    : obj->parent[i - 1][obj->parent[i - 1][j]];
            if (obj->parent[i][j] != -1)
                quit = 0;
        }
        if (quit)
            break;
    }
    return obj;
}

int treeAncestorGetKthAncestor(TreeAncestor *obj, int node, int k)
{
    int max_level = obj->max_level;
    for (int i = 0; i < max_level && node != -1; ++i)
        if (k & (1 << i))
            node = obj->parent[i][node];
    return node;
}


void treeAncestorFree(TreeAncestor *obj)
{
    for (int i = 0; i < obj->max_level; i++)
        free(obj->parent[i]);
    free(obj->parent);
    free(obj);
}


int main()
{
    int tree[100000];
    tree[0] = -1;

    int cnt = 0, num = 0;
    for (int i = 1; i < 100000; i++) {
        tree[i] = num;

        if (++cnt > 1) {
            cnt = 0;
            num++;
        }
    }

    for (int k = 1000; k <= 100000; k += 1000) {
        struct timespec tt1, tt2;
        clock_gettime(CLOCK_MONOTONIC, &tt1);
        TreeAncestor *obj = treeAncestorCreate(k, tree, k);
        for (int i = 0; i < 500; i++) {
            treeAncestorGetKthAncestor(obj, rand() % k, rand() % k);
        }
        clock_gettime(CLOCK_MONOTONIC, &tt2);

        long long time = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                         (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);
        treeAncestorFree(obj);


        clock_gettime(CLOCK_MONOTONIC, &tt1);
        obj = C(k, tree, k);
        for (int i = 0; i < 500; i++) {
            G(obj, rand() % k, rand() % k);
        }
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        long long time2 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);
        F(obj);

        printf("%d, %lld, %lld\n", k, time, time2);
    }


    return 0;
}
