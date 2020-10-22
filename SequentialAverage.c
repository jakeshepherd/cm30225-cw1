#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

double calculateMean(double w, double x, double y, double z) {
    return (w + x + y + z) / 4;
}

double *doAveraging(double *arr, int dimension) {
    double accuracy = 0.01, average;
    bool continueFlag = true;
    int loopCount = 0;

    while (continueFlag == true) {
        continueFlag = false;
        // go through array and calc average for center of block
        for (int i = 1; i < dimension - 1; i++) {
            for (int j = 1; j < dimension - 1; j++) {
                average = calculateMean(
                        arr[dimension * (i - 1) + j],
                        arr[dimension * (i + 1) + j],
                        arr[dimension * i + (j + 1)],
                        arr[dimension * i + (j - 1)]
                );

                // only back out when we reach the correct precision
                if (fabs(arr[dimension * i + j] - average) > accuracy) {
                    arr[dimension * i + j] = average;
                    continueFlag = true;
                }
            }
        }
        loopCount++;
    }

    printf("LOOP COUNT: %d \n", loopCount);

    printf("---------AFTER AVERAGE---------\n");
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            printf("%f, ", arr[dimension * i + j]);
        }
        printf("\n");
    }

    return arr;
}

int main() {
    int randLimit = 100, dimension = 6;
    double *arr = (double *) malloc((unsigned) (dimension * dimension) * sizeof(double));

// use this for putting in custom test data
//    double arr[] = {
//            1.000000, 2.000000, 4.000000, 0.000000, 4.000000, 4.000000,
//            3.000000, 3.000000, 2.000000, 4.000000, 0.000000, 0.000000,
//            1.000000, 2.000000, 1.000000, 1.000000, 0.000000, 2.000000,
//            2.000000, 1.000000, 1.000000, 4.000000, 2.000000, 3.000000,
//            2.000000, 2.000000, 1.000000, 1.000000, 3.000000, 0.000000,
//            2.000000, 1.000000, 1.000000, 3.000000, 4.000000, 2.000000,
//    };

    // fill array with random numbers
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            arr[dimension * i + j] = (double) rand() / (float) (RAND_MAX / randLimit);
        }
    }

    // start the clock for the averaging off the array
    clock_t begin = clock();
    doAveraging(arr, dimension);
    clock_t end = clock();

    printf("TIME TAKEN: %f \n", (double)(end - begin));

    free(arr);

    return 0;
}