#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define ITERATE(i, begin, end) \
    for (int i = (begin), i##_end_ = (end); i < i##_end_; i++)

int *countSubgraphsForEachDiameter(int n,
                                   int **edges,
                                   int edgesSz,
                                   int *edges0sz,
                                   int *rsz)
{
    uint8_t d[n][n]; /* distance */
    memset(d, 0xFF, sizeof(d));
    uint16_t conn[n];

    // clang-format off
    ITERATE(i, 0, n)
        conn[i] = 1 << i;
    // clang-format on

    for (int z = 0; z < edgesSz; z++) {
        int *e = edges[z];
        int u = e[0] - 1, v = e[1] - 1;
        d[u][v] = d[v][u] = 1;
        conn[u] |= 1 << v, conn[v] |= 1 << u;
    }

    // clang-format off
    ITERATE(k, 0, n)
        ITERATE(i, 0, n)
            ITERATE(j, 0, n)
                d[i][j] = MIN(d[i][j], d[i][k] + d[k][j]);
    // clang-format on

    int *rv = calloc(n - 1, sizeof(int));

    ITERATE(S, 1, (1 << n))
    {
        int ctmp = 1 << __builtin_ctz(S);
        while (1) {
            int conn_nxt = ctmp;
            // clang-format off
            ITERATE(d, 0, n)
                if ((ctmp >> d) & 1)
                    conn_nxt |= conn[d];
            // clang-format on
            conn_nxt &= S;
            if (conn_nxt == ctmp)
                break;
            ctmp = conn_nxt;
        }
        if (ctmp ^ S)
            continue;

        uint8_t ret = 0;
        // clang-format off
	ITERATE(i, 0, n)
            if ((S >> i) & 1)
                ITERATE(j, 0, i)
                    if (((S >> j) & 1) && (ret < d[i][j]))
                        ret = d[i][j];
        // clang-format on

        if (!ret)
            continue;
        rv[--ret]++;
    }

    *rsz = n - 1;
    return rv;
}

int main()
{
    int tmp[] = {1, 2, 2, 3, 2, 4};

    int **e;
    e = malloc(3 * sizeof(int *));
    for (int row = 0; row < 3; row++) {
        e[row] = malloc(2 * sizeof(int));
        for (int column = 0; column < 2; column++) {
            e[row][column] = tmp[column + row * 2];
        }
    }

    int sz;
    int *ret = countSubgraphsForEachDiameter(4, e, 3, (int[]){2, 2, 2}, &sz);

    for (int i = 0; i < sz; i++)
        printf("%d ", ret[i]);

    printf("\n");
    free(ret);

    for (int row = 0; row < 3; row++)
        free(e[row]);

    free(e);
    return 0;
}
