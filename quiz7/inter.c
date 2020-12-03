#include <stdbool.h>
#include <stdio.h>
#include <string.h>


bool isInterleave(char *s1, char *s2, char *s3)
{
    size_t n = strlen(s1), m = strlen(s2);
    if ((n + m) != strlen(s3))
        return false;

    bool dp[n + 1][m + 1];
    dp[0][0] = true;

    for (size_t i = 1; i <= n; i++)
        dp[i][0] = dp[i - 1][0] && (s1[i - 1] == s3[i - 1]);

    for (size_t j = 1; j <= n; j++)
        dp[0][j] = dp[0][j - 1] && (s2[j - 1] == s3[j - 1]);


    for (size_t i = 1; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            dp[i][j] = (dp[i][j - 1] && (s2[j - 1] == s3[i + j - 1])) ||
                       (dp[i - 1][j] && (s1[i - 1] == s3[i + j - 1]));
        }
    }

    for (size_t i = 0; i <= n; i++) {
        for (size_t j = 1; j <= m; j++) {
            printf("%d ", dp[i][j]);
        }
        printf("\n");
    }

    return dp[n][m];
}

int main()
{
    printf("\n%d\n", isInterleave("aabcc", "dbbca", "aadbbcbcac"));
    return 0;
}
