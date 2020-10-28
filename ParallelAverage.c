#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

int whileLoopCounter = 0;

struct RowArgs {
    double top, bottom, left, right, middle;
};

void *processRow(void* args) {
    double *values = (double*) args;
    long newAvg = (values[0]+values[1]+values[2]+values[3]) / 4;

//    printf("new average: : %f\n", newAvg);
    pthread_exit((void *) newAvg);
}

void relaxationParallel(
        double *values, // entire 2D array
        int dimension,
        // int numOfThreads,
        double precision
) {
    bool hasConverged = false;
    pthread_t threadList[dimension];
    int rc;

    // create thread pool
    int numberOfThreads = (dimension-1)*(dimension-1);

    while (!hasConverged) {
        for (int i = 1; i < dimension-1; i++) {
            for (int j = 1; j < dimension-1; j++) {
                double args[] = {
                        values[dimension * (i-1) + j],
                        values[dimension * (i+1) + j],
                        values[dimension * i + (j-1)],
                        values[dimension * i + (j+1)]};

                rc = pthread_create(&threadList[i-1], NULL, processRow, (void *)&args);
                if (rc){
                    printf("ERROR; return code from pthread_create() is %d\n", rc);
                    exit(-1);
                }
            }
        }

        // wait for all threads to finish
        for (int i=0; i<dimension; i++) {
            void *returnVal;
            pthread_join(threadList[i], &returnVal);

            printf("FROM THREAD: %ld\n", (long) returnVal);
            hasConverged = true;
        }
    }
}

void printArray(double *arr, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            printf("%f, ", arr[dimension * i + j]);
        }
        printf("\n");
    }
}

int main(void) {
    int arrDim = 6;
    double precision = 0.01;

    printf("\ndimensions: [%dx%d]\n", arrDim, arrDim);
    printf("precision: %f\n\n", precision);

    // unsigned long longDim = (unsigned long) arrDim;
    // double *arr = malloc(longDim * longDim * sizeof(double));
    // populateWithTestData(arr, arrDim);

    double arr[] = {
            2.096136, 0.763294, 0.681351, 0.471130, 1.281154, 1.353358,
            2.892621, 1.282842, 2.719666, 1.423470, 2.259687, 1.566266,
            2.235410, 1.539452, 1.566639, 2.495058, 0.435476, 2.037410,
            0.750322, 1.663466, 0.879023, 1.740040, 0.846988, 0.333857,
            1.138607, 2.561096, 0.339483, 2.686907, 2.853677, 0.757116,
            1.847040, 2.198303, 1.881920, 0.435748, 0.622504, 1.426581
    };

    printf("initial array:\n");
    printArray(arr, arrDim);

    relaxationParallel(arr, arrDim, precision);

    printf("\nresult (after %d iterations):\n", whileLoopCounter);
    printArray(arr, arrDim);
    printf("\n");

    // free(arr);

    return 0;
}