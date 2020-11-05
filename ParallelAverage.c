#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

double *writeVals;
double *readVals;
int dimension;
double precision;
bool hasConverged = false;
int whileLoopCounter = 0;

void arrayCopy(double *arr, double *tempArray, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            tempArray[dimension * i + j] = arr[dimension * i + j];
        }
    }
}

struct ProcessRowArgs {
    int rowStart;
    int rowEnd;
    int threadNum;
};

void setHasConverged(bool b) {
    // pthread_mutex_lock(&convergeMutex);
    hasConverged = b;
    // pthread_mutex_unlock(&convergeMutex);
}

void updateValue(int x, int y) {
    double oldValue = readVals[dimension * x + y];
    double newValue = (
                              readVals[dimension * (x+1) + y] +
                              readVals[dimension * (x-1) + y] +
                              readVals[dimension * x + (y+1)] +
                              readVals[dimension * x + (y-1)]
                      ) / 4;

    if (fabs(oldValue - newValue) > precision) {
        // pthread_mutex_lock(&updateMutex);
        writeVals[dimension * x + y] = newValue;
        setHasConverged(false);
        // pthread_mutex_unlock(&updateMutex);
    }
}

void processRow(void *vArgs) {
    struct ProcessRowArgs *args = (struct ProcessRowArgs*) vArgs;

    for (int i = args->rowStart; i <= args->rowEnd; i++) {
        for (int j = 1; j < dimension - 1; j++) {
            updateValue(i, j);
        }
    }

    printf("> %d.%d - thread done.\n", whileLoopCounter, args->threadNum);

    pthread_barrier_wait(&barrier);
    pthread_exit(NULL);
}

void printArray(double *arr, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            printf("%f, ", arr[dimension * i + j]);
        }
        printf("\n");
    }
}

void relaxParallel(int numOfThreads) {
    int rowsPerThread = (dimension - 2) / numOfThreads;
    printf("\n-> Each thread gets %d rows.", rowsPerThread);

    // init mutex
    // pthread_mutex_init(&updateMutex, NULL);
    // pthread_mutex_init(&convergeMutex, NULL);

    while (!hasConverged) {
        printf("\n===============================================================\n");
        printf("\nIteration %d.\n", whileLoopCounter);
        setHasConverged(true);

        // re-init barrier
        printf("- Init barrier - waiting on %d threads.\n", (unsigned) numOfThreads);
//        pthread_barrier_init (&barrier, NULL, (unsigned) numOfThreads + 1);
        // update read/write vals
        arrayCopy(writeVals, readVals, dimension);

        for (int i = 0; i < numOfThreads; i++) {
            int startRow = rowsPerThread * i + 1;
            int endRow = startRow + (rowsPerThread-1);

            struct ProcessRowArgs *params = malloc(sizeof(struct ProcessRowArgs));
            params->rowStart = startRow;
            params->rowEnd = endRow;
            params->threadNum = i;

            pthread_t thr;
            int cv = pthread_create(&thr, NULL, (void *) processRow, params);
            if (cv != 0) {
                printf("\n[ERROR] error in pthread_create.\n");
                exit(-1);
            }
        }
        // wait for threads to finish
//        pthread_barrier_wait(&barrier);
        printf("- Barrier Hit!\n");

        whileLoopCounter++;
        if (whileLoopCounter >= 100) {
            printf("\n\n!! Max iterations (100) reached.\n\n");
            break;
        }
    }

    // pthread_mutex_destroy(&updateMutex);
    // pthread_mutex_destroy(&convergeMutex);
//    pthread_barrier_destroy(&barrier);
}

double *populateMainArray(double *arr, int dimension) {
    int randLimit = 100;

    // fill array with random numbers
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            arr[dimension * i + j] = (double) rand() / (float) (RAND_MAX / randLimit);
        }
    }

    return arr;
}

int main(void) {
    dimension = 6;
    precision = 0.01;

    writeVals = malloc((unsigned) dimension * (unsigned) dimension * sizeof(double));
    populateMainArray(writeVals, dimension);

    readVals = malloc((unsigned) dimension * (unsigned) dimension * sizeof(double));
    arrayCopy(writeVals, readVals, dimension);

    // copy ready for sequential
    double *seqArr = malloc((unsigned) dimension * (unsigned) dimension * sizeof(double));
    arrayCopy(writeVals, seqArr, dimension);

    printf("\n===============================================================");
    printf("\nInitial array:\n\n");
    printArray(writeVals, dimension);

    printf("\n===============================================================\n");
    printf("Running parallel!");
    clock_t start, end;
    double cpu_time_used;

    int numberOfThreads = 4;
    start = clock();
    relaxParallel(numberOfThreads);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("\n===============================================================");
    printf("\nParallel result (after %d iterations):\n", whileLoopCounter);
    printf("Time: %f\n\n", cpu_time_used);
    printArray(writeVals, dimension);
    printf("\n");

    return 0;
}