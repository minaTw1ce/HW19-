#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to calculate the average of an array
double average(double* data, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

// Function to calculate the median of an array
double median(double* data, int size) {
    // Sort the array in ascending order
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (data[i] > data[j]) {
                // Swap data[i] and data[j]
                double temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }

    // Calculate the median
    if (size % 2 == 0) {
        // If even, average the middle two elements
        return (data[size / 2 - 1] + data[size / 2]) / 2.0;
    } else {
        // If odd, return the middle element
        return data[size / 2];
    }
}

// Function to calculate the standard deviation of an array
double std_dev(double* data, int size, double average) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += pow(data[i] - average, 2);
    }
    return sqrt(sum / size);
}

int main() {
    // Get the name of the data file from the user
    char filename[100];
    printf("Enter the name of the data file: ");
    scanf("%s", filename);

    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file. Exiting.\n");
        return 1;
    }

    // Skip the header row
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);

    // Count the number of rows in the file
    int rows = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        rows++;
    }

    // Rewind the file to read data
    rewind(file);

    // Allocate memory for the data
    double *ampdata = (double *)malloc(rows * sizeof(double));
    double *owdata = (double *)malloc(rows * sizeof(double));
    double *resdata = (double *)malloc(rows * sizeof(double));
    double *twdata = (double *)malloc(rows * sizeof(double));

    // Skip the header row again
    fgets(buffer, sizeof(buffer), file);

    // Read data from the file
    for (int i = 0; i < rows; i++) {
        fscanf(file, "%lf %lf %lf %lf", &ampdata[i], &owdata[i], &resdata[i], &twdata[i]);
    }

    // Close the file
    fclose(file);

    // Calculate and display results
    printf("\nPieces of Data: %d rows\n", rows);
    printf("AMP: Average=%.3fmV, Median=%.3fmV, Standard Deviation=%.3fmV\n",
           average(ampdata, rows), median(ampdata, rows), std_dev(ampdata, rows, average(ampdata, rows)));

    printf("OW: Average=%.3fdB, Median=%.3fdB, Standard Deviation=%.3fdB\n",
           average(owdata, rows), median(owdata, rows), std_dev(owdata, rows, average(owdata, rows)));

    printf("RES: Average=%.3f, Median=%.3f, Standard Deviation=%.3f\n",
           average(resdata, rows), median(resdata, rows), std_dev(resdata, rows, average(resdata, rows)));

    printf("TW: Average=%.3fnm, Median=%.3fnm, Standard Deviation=%.3fnm\n",
           average(twdata, rows), median(twdata, rows), std_dev(twdata, rows, average(twdata, rows)));

    // Free allocated memory
    free(ampdata);
    free(owdata);
    free(resdata);
    free(twdata);

    return 0;
}
