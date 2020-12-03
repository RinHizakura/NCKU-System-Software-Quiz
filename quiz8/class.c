#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
static void solve(int i, int s, int k, int n, int s0, int s1, int *dp)
{
    if ((k == 0) || (i == n)) {
        dp[s0 | s] = min(dp[s0 | s], dp[s0] + 1);
        return;
    }

    solve(i + 1, s, k, n, s0, s1, dp);

    if ((s1 >> i) & 1)
        solve(i + 1, s | 1 << i, k - 1, n, s0, s1, dp);
}

int minNumberOfSemesters(int n,
                         int **dependencies,
                         int dependenciesSize,
                         int *dependenciesColSize,
                         int k)
{
    const size_t cap = 1 << n;
    int pre[cap];
    memset(pre, 0, sizeof(pre));
    for (int i = 0; i < dependenciesSize; i++)
        pre[dependencies[i][1] - 1] |= 1 << (dependencies[i][0] - 1);

    int dp[cap];
    for (int i = 1; i < cap; i++)
        dp[i] = INT_MAX;
    dp[0] = 0;

    for (int s0 = 0; s0 < cap; s0++) {
        // if (dp[s0] == INT_MAX)
        //    continue;

        int s1 = 0;
        for (int i = 0; i < n; i++)
            if (!((s0 >> i) & 1) && ((pre[i] & s0) == pre[i]))
                s1 |= 1 << i;

        printf("%d %d\n", s0, s1);
        solve(0, 0, k, n, s0, s1, dp);
    }

    return dp[cap - 1];
}

int main()
{
    int n = 4;

    int tmp[6] = {2, 1, 3, 1, 1, 4};
    int **dependencies;

    dependencies = malloc(3 * sizeof(int *));
    for (int row = 0; row < 3; row++) {
        dependencies[row] = malloc(2 * sizeof(int));
        for (int column = 0; column < 2; column++) {
            dependencies[row][column] = tmp[column + row * 2];
        }
    }

    int k = 2;

    int dpsize = 2;
    printf("%d\n", minNumberOfSemesters(n, dependencies, 3, &dpsize, k));

    for (int row = 0; row < 3; row++)
        free(dependencies[row]);

    free(dependencies);
    return 0;
}
