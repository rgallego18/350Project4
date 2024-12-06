//Author: Jayson Boubin -- Fall 2022
#include "matmul.h"

int* mat1 = NULL;
int* mat2 = NULL;
int* matRes = NULL;
int* matSol = NULL;

// Thread arguments structure
typedef struct {
    int startRow;
    int endRow;
} ThreadArgs;

// Single-threaded matrix multiplication
void multiply() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            matRes[i * K + j] = 0; // Initialize result matrix cell
            for (int k = 0; k < N; k++) {
                matRes[i * K + j] += mat1[i * N + k] * mat2[k * K + j];
            }
        }
    }
}

// Multi-threaded matrix multiplication
void* multiplyRows(void* args) {
    ThreadArgs* targs = (ThreadArgs*)args;
    int startRow = targs->startRow;
    int endRow = targs->endRow;

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < K; j++) {
            matRes[i * K + j] = 0; // Initialize result matrix cell
            for (int k = 0; k < N; k++) {
                matRes[i * K + j] += mat1[i * N + k] * mat2[k * K + j];
            }
        }
    }
    return NULL;
}

void multiplyWithThreads(int nThreads) {
    pthread_t threads[nThreads];
    ThreadArgs targs[nThreads];
    int rowsPerThread = M / nThreads; // Divide rows evenly
    int remainingRows = M % nThreads; // Handle leftover rows

    // Create threads and assign rows
    for (int t = 0; t < nThreads; t++) {
        targs[t].startRow = t * rowsPerThread;
        targs[t].endRow = (t + 1) * rowsPerThread;

        if (t == nThreads - 1) {
            targs[t].endRow += remainingRows; // Add leftover rows to the last thread
        }

        pthread_create(&threads[t], NULL, multiplyRows, &targs[t]);
    }

    // Wait for all threads to complete
    for (int t = 0; t < nThreads; t++) {
        pthread_join(threads[t], NULL);
    }
}



//--- DO NOT MODIFY BELOW HERE ---
int main(int argc, char* argv[])
{
    
    struct timeval start, end;
    double mult_time;

    if(argc != 2) {
        printf("Please provide 1 argument: #threads (int)\n");
        exit(-1);
    }

    int nthreads = atoi(argv[1]);
    
    if(nthreads <= 0){
        printf("Please enter a correct value for #threads\n");
        exit(-1);
    }
    printf("--- Reading in Matrices ---\n");

    mat1 = initialize(M, N, MAT_A);
    mat2 = initialize(N, K, MAT_B);
    matSol = initialize(M, K, MAT_C);
    matRes = malloc(M * K * sizeof(int)); 

    printf("--- Matrices Successfully Read, Multiplying Now ---\n");

    gettimeofday(&start, NULL);
    if(nthreads > 1){
        printf("--- Multiplying with %d threads ---\n", nthreads);
        multiplyWithThreads(nthreads);
    } else {
        printf("--- Multiplying with a single thread ---\n");
        multiply();
    }
    
    gettimeofday(&end, NULL); 
    mult_time = getTotalTime(start, end);

    if(compare()){
        printf("Correct! Your matrices multiplied properly\n");
    } else {
        printf("Incorrect! Your matrices did not multiply properly. Check your solution and try again.\n");
    }
    
    printf("This operation took %f seconds\n", mult_time);

    //Don't forget to free allocated heap memory!
    
    free(mat1);
    free(mat2);
    free(matRes);
    free(matSol);
    
    return 0;
}
