#include <stdio.h>
#include <math.h>

#define INTEGRATE_LOW   -2.0		// lower integration bound
#define INTEGRATE_HIGH   2.0		// upper integration bound
#define n_LOW			   0		// lower value on interval numbers for simpson
#define n_HIGH			   8		// upper value on interval numbers for simpson


// declare and define the function to be integrated
double f(double x) {
	if ( -1.0 <= x && x <= 1.0 ) {
		return  -x*x*x + 1.0;				// if x \in dom(f): f(x) = -x^3 + 1
	}
	else {
		return 0.0;							// if x \notin dom(f): f(x) = 0
	}
}

// declare and define the simpson rule for discrete integration
double SimpsonIntegrate(const double a, const double b, const int n, double (*function)(double)) {

	double h       = pow(2, -n);					// interval length
	int    N       = (b - a)/h;						// number of discrete intervals
	double sum_one = 0.0;							// initialize first sum
	double sum_two = 0.0;							// initialize second_sum
	int    i;										// index for the sum
	double xi_1    = a;								// x_{i-1}, initialized with lower integration bound
	double xi;										// x_i

	for (i = 1; i <= N-1; i++ ) {					// do the sum from i = 1 to i = N-1
		xi       = xi_1 + h;						// step forward
		sum_one += function(xi);					// evaluate the function
		sum_two += function( ( xi_1 + xi )/2 );		// evaluate the function at intermediate point
		xi_1     = xi;								// update the step
	}
	xi       = xi_1 + h;							// step forward
	sum_two += function( ( xi_1 + xi )/2 );			// evaluate and sum for i = N

	return h*( function(a) + 2*sum_one + function(b) + 4*sum_two )/6;
}

int main(void) {

	int    n;			// declare the discretizzation parameter
	double intg_val;	// declate variable for the value of the approx. integral

	for ( n = n_LOW; n <= n_HIGH; n++ ) {
		intg_val = SimpsonIntegrate(INTEGRATE_LOW, INTEGRATE_HIGH, n, f);	// compute
		printf("for n = %d, S(f) = %.3f\n", n, intg_val);					// print
	}
}
