#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_q(char **q, int n)
{
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++)
            printf("%c ", q[j][k]);
        printf("\n");
    }

    printf("\n");
}

static int sol = 0;
int sizes[] = {1,      1,       0,        0,        2,        10,    4,
               40,     92,      352,      724,      2680,     14200, 73712,
               365596, 2279184, 14772512, 95815104, 666090624};

void recursive_solver(int n,
                      int row,
                      char **queen,
                      char ***p,
                      int maj_con,
                      int min_con,
                      int col_con)
{
    int queen_pos;
    int conflicts = min_con | maj_con | col_con;

    min_con &= 65535;
    for (int col = 0; col < n; col++) {
        queen_pos = 1 << (16 - col);
        if (!(queen_pos & conflicts)) {
            if (row == n - 1) {
                queen[row][col] = 'Q';
                for (int i = 0; i < n; i++)
                    memcpy(p[sol][i], queen[i], n + 1);
                sol++;
                queen[row][col] = '.';

            } else {
                queen[row][col] = 'Q';
                recursive_solver(
                    n, row + 1, queen, p, (maj_con | queen_pos) >> 1,
                    (min_con | queen_pos) << 1, (col_con | queen_pos));
                queen[row][col] = '.';
            }
        }
    }
}


char ***solveNQueens(int n, int *returnSize, int **returnColumnSizes)
{
    sol = 0;
    *returnColumnSizes = malloc(sizes[n] * sizeof(int));
    *returnSize = sizes[n];
    for (int i = 0; i < sizes[n]; i++)
        (*returnColumnSizes)[i] = n;

    char ***p;
    p = malloc(sizes[n] * sizeof(char **));

    for (int i = 0; i < sizes[n]; ++i)
        p[i] = malloc(n * sizeof(char *));


    for (int i = 0; i < sizes[n]; ++i)
        for (int j = 0; j < n; ++j) {
            p[i][j] = malloc((n + 1) * sizeof(char));
        }

    char **queen;
    queen = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        queen[i] = malloc((n + 1) * sizeof(char));
        for (int j = 0; j < n; j++)
            queen[i][j] = '.';
        queen[i][n] = '\0';
    }

    recursive_solver(n, 0, queen, p, 0, 0, 0);

    for (int i = 0; i < n; i++)
        free(queen[i]);
    free(queen);

    return p;
}

void free_data(char ***data, int n)
{
    for (int i = 0; i < sizes[n]; ++i) {
        for (int j = 0; j < n; ++j)
            free(data[i][j]);
        free(data[i]);
    }

    free(data);
}


int main()
{
    int n = 4;
    int size = 0;
    int *colsize;

    char ***s = solveNQueens(n, &size, &colsize);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < colsize[i]; j++) {
            printf("%s\n", s[i][j]);
        }
        printf("\n");
    }

    free(colsize);
    free_data(s, n);
}
