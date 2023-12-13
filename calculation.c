#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//This function sorts an array x with size values into ascending order.
void sortascend(double x[],int size)
{
int k, j, m;
    double temp;
    /*  Implement selection sort algorithm.  */
    for (k=0; k<=size-2; k++)
        {
            /*  Exchange minimum with next array value.  */
            m = k;
            for (j=k+1; j<=size-1; j++)
            if (x[j] < x[m])
            m = j;
            temp = x[m];
            x[m] = x[k];
            x[k] = temp;
        }
    /*  Void return.  */
    return;
}

// Function to calculate the average of an array
double average(double* data,int size)
{
    int i;
        double sum=0;
        /*  Determine mean value.  */
        for (i=0; i<=size-1; i++)
            sum += data[i];
        /*  Return mean value.  */
        return (sum/size);
}

// Function to calculate the median of an array
double median(double* data, int size) 
{
    /*  Declare variables.  */
        int i;
        double middle, *temp;
        /* declare memory for the temporary array */
        temp = (double *)calloc(size,sizeof(double));
        if (temp==NULL)
        {
                printf("Error allocating temporary array");
                return (0);
        }
        /* Initialize the temp with the values in x */
        for (i=0;i<=size-1;i++) temp[i]=data[i];
        /* Sort the temp */
        sortascend(temp,size);
        /*  Determine median value.  */
        i = (size/2);
        if (size%2 != 0){
            middle = temp[i];
        }
           
        else
        {
            middle = (temp[i-1] + temp[i])/2.0;
        }
        /*  Return median value.  */
        free(temp);
        return (middle);
}
 // This function returns the variance of an array x with size elements.
double variance(double x[],int size)
{
        /*  Declare variables   */
        int k;
        double sum=0, mu;
        /*  Determine variance.  */
        mu = average(x,size);
        for (k=0; k<=size-1; k++)
        sum += (x[k] - mu)*(x[k] - mu);
        /*  Return variance.  */
        return (sum/(size-1));
    }

// Function to calculate the standard deviation of an array
double std_dev(double* data, int size) 
{
    return (sqrt(variance(data,size)));
}

int main() 
{
    // Get the name of the data file from the user
    char filename[100];
    printf("Enter the name of the data file: ");
    scanf("%s", filename);

    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        printf("Error opening file.\n");
        return 1;
    }

    // Skip the header row
    char buffer[100];
    fgets(buffer, sizeof(buffer), file);

    // Count the number of rows in the file
    int rows = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) 
    {
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
    for (int i = 0; i < rows; i++) 
    {
        fscanf(file, "%lf %lf %lf %lf", &ampdata[i], &owdata[i], &resdata[i], &twdata[i]);
    }

    // Close the file
    fclose(file);

    // Calculate and display results
    printf("\nPieces of Data: %d rows\n", rows);
    printf("AMP: Average = %.2fmV, Median = %.2fmV, Standard Deviation = %.2fmV\n",
           average(ampdata, rows), median(ampdata, rows), std_dev(ampdata, rows));

    printf("OW: Average = %.2fdB, Median = %.2fdB, Standard Deviation = %.2fdB\n",
           average(owdata, rows), median(owdata, rows), std_dev(owdata, rows));

    printf("RES: Average = %.2f, Median = %.2f, Standard Deviation = %.2f\n",
           average(resdata, rows), median(resdata, rows), std_dev(resdata, rows));

    printf("TW: Average = %.2fnm, Median = %.2fnm, Standard Deviation = %.2fnm\n",
           average(twdata, rows), median(twdata, rows), std_dev(twdata, rows));

    // Free allocated memory
    free(ampdata);
    free(owdata);
    free(resdata);
    free(twdata);
    return 0;
}
