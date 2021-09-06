/*
 * Description: ISYS C Course Project 3
 *
 * This code implements a IIR filter
 *
 */

/*
 * authors Schmidgall, Werner
 * date (due) 08.01.2021
 */

/* Settings */
#define INPUT_FILE "csv_test_data.txt"
#define OUTPUT_FILE "csv_result.txt"
#define MAXCHAR 80

// === standard libraries ===
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// === external includes ===
#include "coeff_iir.h"
#include "matrixmath.h"

// user defined types
typedef struct{
	double t;	// time stamp
	double u;	// input to filter u[k]
	double y;	// output of filter y[k]
} point_t;

int main(void) {

	/*
	 * remark:
	 * the following could be done in one for loop, but for readability
	 * it is executed in three seperate loops, one to read the data,
	 * one to calculate the filter output and finally one to write the results
	 */

	// === declare filter variables ===
	const double       a[]     = {NENNER_COEFF_VECTOR};				// denum coeffs
	const double       b[]     = {ZAEHLER_COEFF_VECTOR};			// num coeffs
	const unsigned int n       = sizeof(a) / sizeof(a[0]) - 1;		// filter order

	// === read csv_test_data.txt ===
	char         str[MAXCHAR];										// max number of chars in input string
	unsigned int point_counter 	= 0;								// counter for the number of data points in input file
	char*        token;												// tmp data
	unsigned int k 				= 0;								// loop int
	FILE* 		 fp 			= fopen(INPUT_FILE, "r");			// input file

	if (fp == NULL) {printf("Couldn't open file %s \n", INPUT_FILE); exit(EXIT_FAILURE);};

	// determine size of input data
	while (fgets(str, MAXCHAR, fp) != NULL) {point_counter++;};				// count number of input data
	rewind(fp);
	// allocate memory for input data
	point_t* points = (point_t*)malloc(point_counter*sizeof(point_t));
	// read and store data
	while (fgets(str, MAXCHAR, fp) != NULL) {
		// read and store time stamp
		token = strtok(str, ",");
		points[k].t = strtod(token, NULL);
		// read and store input value
		token = strtok(NULL, "\n");
		points[k].u = strtod(token, NULL);
		k++;
	}
	// close file
	fclose(fp);

	// === simulate time series and calculate filter output ===
	double* u_buffer = (double*)malloc(n*sizeof(double));					// buffer to save u[k-i], i \in [1,n]
	double* y_buffer = (double*)malloc(n*sizeof(double));					// buffer to save y[k-i], i \in [1,n]
	// initialize buffer to zero
	for ( k = 0; k < n; k++) {u_buffer[k] = 0.0; y_buffer[k] = 0.0;}
	unsigned int j = 0;		// current index in buffer \in [0,n]
	unsigned int i;			// counts from 1 to n
	int l;					// counts backwards from j trough buffer
	double yk;				// current filter output
	double uk;				// current filter input

	for (k = 0; k < point_counter; k++) {
		// filter equation :
		// a[0]*y[k] = b[0]*u[k] + sum_{i = 1}^n b[i]*u[k-1] - a[i]*y[k-i]

		// receive measurement
		uk = points[k].u;
		// calculate filtered output
		yk = b[0]*uk;
		for (i = 1; i < n+1; i++) {
			l = j - i;
			if (l < 0) {l += n;};
			yk += b[i]*u_buffer[l] - a[i]*y_buffer[l];
		}
		// save data
		points[k].y = a[0]*yk; // a[0] = 1 if normalized. Add it for good measure
		// save last n data in buffer
		u_buffer[j] = uk;
		y_buffer[j] = yk;
		// update buffer index
		j ++;
		j = j%n;
	}

	// === write solutions to csv_result.txt ===
	fp = fopen(OUTPUT_FILE, "w");
	if (fp == NULL) {
		free(points);
		free(u_buffer);
		free(y_buffer);
		printf("Couldn't open file %s \n", OUTPUT_FILE);
		exit(EXIT_FAILURE);
	}

	for (k = 0; k < point_counter; k++) {
		fprintf(fp, "%f,%f,%f\n", points[k].t, points[k].u, points[k].y);
	}
	fclose(fp);

	// free memory
	free(points);
	free(u_buffer);
	free(y_buffer);

	return EXIT_SUCCESS;
}
