#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

double calculateMean(double w, double x, double y, double z) {
    return (w + x + y + z) / 4;
}

double *populateMainArray(int dimension) {
    int randLimit = 100;
    double *arr = malloc((unsigned long) dimension * (unsigned long) dimension * sizeof(double));

    // fill array with random numbers
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            arr[dimension * i + j] = (double) rand() / (float) (RAND_MAX / randLimit);
        }
    }

    return arr;
}

void populateTempArray(double *arr, double *tempArray, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            tempArray[dimension * i + j] = arr[dimension * i + j];
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

void relaxationMethod(double *arr, int dimension) {
    int loopCount = 0;
    double accuracy = 0.01;
    bool precisionNotReached = true;
    double *tempArray = malloc((unsigned long) dimension * (unsigned long) dimension * sizeof(double));

    printf("---------BEFORE AVERAGE---------\n");
    printArray(arr, dimension);

    while (precisionNotReached) {
        precisionNotReached = false;
        loopCount++;

        // take a copy of the original array so that all the values for the average are the original
        populateTempArray(arr, tempArray, dimension);

        // go through array and calc average for center of block
        for (int i = 1; i < dimension - 1; i++) {
            for (int j = 1; j < dimension - 1; j++) {
                double average = calculateMean(
                        tempArray[dimension * (i - 1) + j],
                        tempArray[dimension * (i + 1) + j],
                        tempArray[dimension * i + (j + 1)],
                        tempArray[dimension * i + (j - 1)]
                );

                /**
                 * only back out when we reach the correct precision
                 * and replace old center value with the new average
                 */
                if (fabs(tempArray[dimension * i + j] - average) > accuracy) {
                    arr[dimension * i + j] = average;
                    precisionNotReached = true;
                }
            }
        }
    }

    printf("LOOP COUNT: %d \n", loopCount);
    printf("---------AFTER AVERAGE---------\n");
    printArray(arr, dimension);

    free(tempArray);
}

int main(int argc, char **argv) {
    int dimension;
    if (argc > 1) {
        dimension = atoi(argv[1]);
    } else {
        dimension = 6;
    }

    double *arr = populateMainArray(dimension);

// use this for putting in custom test data
//    double arr[] = {
//            2.096136, 0.763294, 0.681351, 0.471130, 1.281154, 1.353358,
//            2.892621, 1.282842, 2.719666, 1.423470, 2.259687, 1.566266,
//            2.235410, 1.539452, 1.566639, 2.495058, 0.435476, 2.037410,
//            0.750322, 1.663466, 0.879023, 1.740040, 0.846988, 0.333857,
//            1.138607, 2.561096, 0.339483, 2.686907, 2.853677, 0.757116,
//            1.847040, 2.198303, 1.881920, 0.435748, 0.622504, 1.426581
//    };

    // start the clock for the averaging off the array
    clock_t begin = clock();
    relaxationMethod(arr, dimension);
    clock_t end = clock();

    printf("TIME TAKEN: %f \n", (double) (end - begin) / CLOCKS_PER_SEC);

    free(arr);

    return 0;
}