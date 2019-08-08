/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    /* use the stuff already in the cache as possible */
    /* conflict misses especially along the diagonal. */
    int i, j;
    if (M == 32 && N == 32) {
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 8) {
                B[i + 0][j] = A[j][i + 0];
                B[i + 0][j + 1] = A[j + 1][i + 0];
                B[i + 0][j + 2] = A[j + 2][i + 0];
                B[i + 0][j + 3] = A[j + 3][i + 0];
                B[i + 0][j + 4] = A[j + 4][i + 0];
                B[i + 0][j + 5] = A[j + 5][i + 0];
                B[i + 0][j + 6] = A[j + 6][i + 0];
                B[i + 0][j + 7] = A[j + 7][i + 0];

                B[i + 1][j + 1] = A[j + 1][i + 1];
                B[i + 1][j] = A[j][i + 1];
                B[i + 1][j + 2] = A[j + 2][i + 1];
                B[i + 1][j + 3] = A[j + 3][i + 1];
                B[i + 1][j + 4] = A[j + 4][i + 1];
                B[i + 1][j + 5] = A[j + 5][i + 1];
                B[i + 1][j + 6] = A[j + 6][i + 1];
                B[i + 1][j + 7] = A[j + 7][i + 1];

                B[i + 2][j + 2] = A[j + 2][i + 2];
                B[i + 2][j] = A[j][i + 2];
                B[i + 2][j + 1] = A[j + 1][i + 2];
                B[i + 2][j + 3] = A[j + 3][i + 2];
                B[i + 2][j + 4] = A[j + 4][i + 2];
                B[i + 2][j + 5] = A[j + 5][i + 2];
                B[i + 2][j + 6] = A[j + 6][i + 2];
                B[i + 2][j + 7] = A[j + 7][i + 2];

                B[i + 3][j + 3] = A[j + 3][i + 3];
                B[i + 3][j] = A[j][i + 3];
                B[i + 3][j + 1] = A[j + 1][i + 3];
                B[i + 3][j + 2] = A[j + 2][i + 3];
                B[i + 3][j + 4] = A[j + 4][i + 3];
                B[i + 3][j + 5] = A[j + 5][i + 3];
                B[i + 3][j + 6] = A[j + 6][i + 3];
                B[i + 3][j + 7] = A[j + 7][i + 3];

                B[i + 4][j + 4] = A[j + 4][i + 4];
                B[i + 4][j] = A[j][i + 4];
                B[i + 4][j + 1] = A[j + 1][i + 4];
                B[i + 4][j + 2] = A[j + 2][i + 4];
                B[i + 4][j + 3] = A[j + 3][i + 4];
                B[i + 4][j + 5] = A[j + 5][i + 4];
                B[i + 4][j + 6] = A[j + 6][i + 4];
                B[i + 4][j + 7] = A[j + 7][i + 4];

                B[i + 5][j + 5] = A[j + 5][i + 5];
                B[i + 5][j] = A[j][i + 5];
                B[i + 5][j + 1] = A[j + 1][i + 5];
                B[i + 5][j + 2] = A[j + 2][i + 5];
                B[i + 5][j + 3] = A[j + 3][i + 5];
                B[i + 5][j + 4] = A[j + 4][i + 5];
                B[i + 5][j + 6] = A[j + 6][i + 5];
                B[i + 5][j + 7] = A[j + 7][i + 5];

                B[i + 6][j + 6] = A[j + 6][i + 6];
                B[i + 6][j] = A[j][i + 6];
                B[i + 6][j + 1] = A[j + 1][i + 6];
                B[i + 6][j + 2] = A[j + 2][i + 6];
                B[i + 6][j + 3] = A[j + 3][i + 6];
                B[i + 6][j + 4] = A[j + 4][i + 6];
                B[i + 6][j + 5] = A[j + 5][i + 6];
                B[i + 6][j + 7] = A[j + 7][i + 6];
                
                B[i + 7][j + 7] = A[j + 7][i + 7];
                B[i + 7][j] = A[j][i + 7];
                B[i + 7][j + 1] = A[j + 1][i + 7];
                B[i + 7][j + 2] = A[j + 2][i + 7];
                B[i + 7][j + 3] = A[j + 3][i + 7];
                B[i + 7][j + 4] = A[j + 4][i + 7];
                B[i + 7][j + 5] = A[j + 5][i + 7];
                B[i + 7][j + 6] = A[j + 6][i + 7];
            }
        }
    }
    
    /* do not pay too much attention on the tiny point (eg. diag) */
    /* do great effort to optimize the main portion (eg. non-diag) */
    /* 8 x 8 certainly out-performs the 4x4 case */
    if (M == 64 && N == 64) { 
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 8) {
                int k, l, t1, t2, t3, t4, t5, t6, t7, t8;
                /* optimize for the diag */
                for (k = 0; k < 4; k++) {   /* reverse upper half */
                    t1 = A[i + k][j + 0];
                    t2 = A[i + k][j + 1];
                    t3 = A[i + k][j + 2];
                    t4 = A[i + k][j + 3];
                    t5 = A[i + k][j + 4];
                    t6 = A[i + k][j + 5];
                    t7 = A[i + k][j + 6];
                    t8 = A[i + k][j + 7];

                    B[j][i + k] = t1;
                    B[j + 1][i + k] = t2;
                    B[j + 2][i + k] = t3;
                    B[j + 3][i + k] = t4;
                    B[j][i + k + 4] = t5;
                    B[j + 1][i + k + 4] = t6;
                    B[j + 2][i + k + 4] = t7;
                    B[j + 3][i + k + 4] = t8;
                }
                for (l = j + 4; l < j + 8; l++) {
                    t5 = A[i + 4][l - 4];
                    t6 = A[i + 5][l - 4];
                    t7 = A[i + 6][l - 4];
                    t8 = A[i + 7][l - 4];

                    t1 = B[l - 4][i + 4];
                    t2 = B[l - 4][i + 5];
                    t3 = B[l - 4][i + 6];
                    t4 = B[l - 4][i + 7];

                    // no cache miss
                    B[l - 4][i + 4] = t5;
                    B[l - 4][i + 5] = t6;
                    B[l - 4][i + 6] = t7;
                    B[l - 4][i + 7] = t8;

                    B[l][i + 0] = t1;
                    B[l][i + 1] = t2;
                    B[l][i + 2] = t3;
                    B[l][i + 3] = t4;

                    B[l][i + 4] = A[i + 4][l];
                    B[l][i + 5] = A[i + 5][l];
                    B[l][i + 6] = A[i + 6][l];
                    B[l][i + 7] = A[i + 7][l];
                }
            }
        }
    }
    /* first think of a just correct version */
    if (M == 61 && N == 67) {
        int k, l, size = 23;
        for (i = 0; i < N; i += size) {
            for (j = 0; j < M; j += size) {
                for (k = i; k < i + size && k < N; k++) {
                    for (l = j; l < j + size && l < M; l++) {
                        B[l][k] = A[k][l];
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

