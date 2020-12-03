#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

int connectTwoGroups(int **cost, int costSize, int *costColSize)
{
    // size of first group
    const int size1 = costSize;
    // size of second group
    const int size2 = costColSize[0];
    // size of mask
    const int mask_size = (1 << size2) - 1;

    int min_table[size2];
    // init dp table with all zero
    unsigned int dp[size1 + 1][mask_size + 1];

    // for the first round, pick up all min edge for each node in group 2
    for (int j = 0; j < size2; j++) {
        int min = 200;
        for (int i = 0; i < size1; i++) {
            if (cost[i][j] < min) {
                min = cost[i][j];
            }
        }
        min_table[j] = min;
    }

    // then update dp table:
    // count cost for each mask for choosing jth node
    // which the jth bit of mask is set to 1
    memset(dp[0], 0, sizeof(int) * (mask_size + 1));
    for (int m = 0; m <= mask_size; m++) {
        for (int j = 0; j < size2; j++) {
            if (m & (1 << j))
                dp[0][m] += min_table[j];
        }
    }

    // now, we already find the edge which let all the node of group2 connect to
    // group1 for the next step, we should check the node in group1 now

    // for dp[i][m], it means the min cost when node from  1 to i in group 1
    // connect with node in group2 of the mask's jth bit of mask is set to 1
    for (int i = 1; i <= size1; i++) {
        for (int m = 0; m <= mask_size; m++) {
            dp[i][m] = INT_MAX;
            for (int j = 0; j < size2; j++) {
                // if j is already the min edge of node in group2, the min cost
                // should consider the combination without node j in group add
                // this edge
                if (m & (1 << j)) {
                    dp[i][m] = min(dp[i][m],
                                   cost[i - 1][j] + dp[i - 1][m & ~(1 << j)]);
                }
                // or we just consider add this edge directly
                else {
                    dp[i][m] = min(dp[i][m], cost[i - 1][j] + dp[i - 1][m]);
                }
            }
        }
    }

    return dp[size1][mask_size];
}

int main()
{
    // int tmp[12] = {93,56,92,53,44,18,86,44,69,54,60,30};
    int tmp[9] = {1, 3, 5, 4, 1, 1, 1, 5, 3};
    int **cost;

    cost = malloc(3 * sizeof(int *));
    for (int row = 0; row < 3; row++) {
        cost[row] = malloc(3 * sizeof(int));
        for (int column = 0; column < 3; column++) {
            cost[row][column] = tmp[column + row * 3];
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            printf("%d ", cost[i][j]);
        printf("\n");
    }
    printf("\n");
    int costColSize[] = {3, 3, 3};
    printf("%d \n", connectTwoGroups(cost, 3, costColSize));
}
