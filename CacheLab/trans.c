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
    int ibase, jbase;
    if (M == 32 && N == 32) {
        for (ibase = 0; ibase < N; ibase += 8) {
            for (jbase = 0; jbase < M; jbase += 8) {
                B[ibase + 0][jbase] = A[jbase][ibase + 0];
                B[ibase + 0][jbase + 1] = A[jbase + 1][ibase + 0];
                B[ibase + 0][jbase + 2] = A[jbase + 2][ibase + 0];
                B[ibase + 0][jbase + 3] = A[jbase + 3][ibase + 0];
                B[ibase + 0][jbase + 4] = A[jbase + 4][ibase + 0];
                B[ibase + 0][jbase + 5] = A[jbase + 5][ibase + 0];
                B[ibase + 0][jbase + 6] = A[jbase + 6][ibase + 0];
                B[ibase + 0][jbase + 7] = A[jbase + 7][ibase + 0];

                B[ibase + 1][jbase + 1] = A[jbase + 1][ibase + 1];
                B[ibase + 1][jbase] = A[jbase][ibase + 1];
                B[ibase + 1][jbase + 2] = A[jbase + 2][ibase + 1];
                B[ibase + 1][jbase + 3] = A[jbase + 3][ibase + 1];
                B[ibase + 1][jbase + 4] = A[jbase + 4][ibase + 1];
                B[ibase + 1][jbase + 5] = A[jbase + 5][ibase + 1];
                B[ibase + 1][jbase + 6] = A[jbase + 6][ibase + 1];
                B[ibase + 1][jbase + 7] = A[jbase + 7][ibase + 1];

                B[ibase + 2][jbase + 2] = A[jbase + 2][ibase + 2];
                B[ibase + 2][jbase] = A[jbase][ibase + 2];
                B[ibase + 2][jbase + 1] = A[jbase + 1][ibase + 2];
                B[ibase + 2][jbase + 3] = A[jbase + 3][ibase + 2];
                B[ibase + 2][jbase + 4] = A[jbase + 4][ibase + 2];
                B[ibase + 2][jbase + 5] = A[jbase + 5][ibase + 2];
                B[ibase + 2][jbase + 6] = A[jbase + 6][ibase + 2];
                B[ibase + 2][jbase + 7] = A[jbase + 7][ibase + 2];

                B[ibase + 3][jbase + 3] = A[jbase + 3][ibase + 3];
                B[ibase + 3][jbase] = A[jbase][ibase + 3];
                B[ibase + 3][jbase + 1] = A[jbase + 1][ibase + 3];
                B[ibase + 3][jbase + 2] = A[jbase + 2][ibase + 3];
                B[ibase + 3][jbase + 4] = A[jbase + 4][ibase + 3];
                B[ibase + 3][jbase + 5] = A[jbase + 5][ibase + 3];
                B[ibase + 3][jbase + 6] = A[jbase + 6][ibase + 3];
                B[ibase + 3][jbase + 7] = A[jbase + 7][ibase + 3];

                B[ibase + 4][jbase + 4] = A[jbase + 4][ibase + 4];
                B[ibase + 4][jbase] = A[jbase][ibase + 4];
                B[ibase + 4][jbase + 1] = A[jbase + 1][ibase + 4];
                B[ibase + 4][jbase + 2] = A[jbase + 2][ibase + 4];
                B[ibase + 4][jbase + 3] = A[jbase + 3][ibase + 4];
                B[ibase + 4][jbase + 5] = A[jbase + 5][ibase + 4];
                B[ibase + 4][jbase + 6] = A[jbase + 6][ibase + 4];
                B[ibase + 4][jbase + 7] = A[jbase + 7][ibase + 4];

                B[ibase + 5][jbase + 5] = A[jbase + 5][ibase + 5];
                B[ibase + 5][jbase] = A[jbase][ibase + 5];
                B[ibase + 5][jbase + 1] = A[jbase + 1][ibase + 5];
                B[ibase + 5][jbase + 2] = A[jbase + 2][ibase + 5];
                B[ibase + 5][jbase + 3] = A[jbase + 3][ibase + 5];
                B[ibase + 5][jbase + 4] = A[jbase + 4][ibase + 5];
                B[ibase + 5][jbase + 6] = A[jbase + 6][ibase + 5];
                B[ibase + 5][jbase + 7] = A[jbase + 7][ibase + 5];

                B[ibase + 6][jbase + 6] = A[jbase + 6][ibase + 6];
                B[ibase + 6][jbase] = A[jbase][ibase + 6];
                B[ibase + 6][jbase + 1] = A[jbase + 1][ibase + 6];
                B[ibase + 6][jbase + 2] = A[jbase + 2][ibase + 6];
                B[ibase + 6][jbase + 3] = A[jbase + 3][ibase + 6];
                B[ibase + 6][jbase + 4] = A[jbase + 4][ibase + 6];
                B[ibase + 6][jbase + 5] = A[jbase + 5][ibase + 6];
                B[ibase + 6][jbase + 7] = A[jbase + 7][ibase + 6];
                
                B[ibase + 7][jbase + 7] = A[jbase + 7][ibase + 7];
                B[ibase + 7][jbase] = A[jbase][ibase + 7];
                B[ibase + 7][jbase + 1] = A[jbase + 1][ibase + 7];
                B[ibase + 7][jbase + 2] = A[jbase + 2][ibase + 7];
                B[ibase + 7][jbase + 3] = A[jbase + 3][ibase + 7];
                B[ibase + 7][jbase + 4] = A[jbase + 4][ibase + 7];
                B[ibase + 7][jbase + 5] = A[jbase + 5][ibase + 7];
                B[ibase + 7][jbase + 6] = A[jbase + 6][ibase + 7];
            }
        }
    }
    if (M == 64 && N == 64) {
        for (ibase = 0; ibase < N - 4; ibase += 4) {
            
        }
        for (ibase = 0; ibase < N; ibase += 4) {
            for (jbase = 0; jbase < M; jbase += 4) {
                if ((ibase != N - 4 && jbase != M - 4) && 
                (ibase == jbase || ibase == 4 + jbase || ibase + 4 == jbase)) {
                    continue;
                }
                B[ibase + 0][jbase] = A[jbase][ibase + 0];
                B[ibase + 0][jbase + 1] = A[jbase + 1][ibase + 0];
                B[ibase + 0][jbase + 2] = A[jbase + 2][ibase + 0];
                B[ibase + 0][jbase + 3] = A[jbase + 3][ibase + 0];

                B[ibase + 1][jbase + 1] = A[jbase + 1][ibase + 1];
                B[ibase + 1][jbase] = A[jbase][ibase + 1];
                B[ibase + 1][jbase + 2] = A[jbase + 2][ibase + 1];
                B[ibase + 1][jbase + 3] = A[jbase + 3][ibase + 1];

                B[ibase + 2][jbase + 2] = A[jbase + 2][ibase + 2];
                B[ibase + 2][jbase] = A[jbase][ibase + 2];
                B[ibase + 2][jbase + 1] = A[jbase + 1][ibase + 2];
                B[ibase + 2][jbase + 3] = A[jbase + 3][ibase + 2];

                B[ibase + 3][jbase + 3] = A[jbase + 3][ibase + 3];
                B[ibase + 3][jbase] = A[jbase][ibase + 3];
                B[ibase + 3][jbase + 1] = A[jbase + 1][ibase + 3];
                B[ibase + 3][jbase + 2] = A[jbase + 2][ibase + 3];
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
