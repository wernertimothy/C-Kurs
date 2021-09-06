/*
 * Description: ISYS C Course Project 2
 *
 * This code performes a discrete integration of 3 different systems of the form
 * dx = Ax
 * using the classical Runge-Kutta Scheme.
 *
 */

/*
 * authors Schmidgall, Werner
 * date (due) 04.12.2020
 */

// === standard libraries ===
#include <stdio.h>
#include "matrixmath.h"
// === functions ===
#include "f.h"
#include "define_system.h"
#include "Ruku_integrate.h"

/* Settings: */
#define STEPSIZE           0.05		// step size of Runge-Kutta Integrator
#define SIMULATIONTIME     10.0		// horizon of integration
#define SYSTEMNUMBER       1        // choose 1, 2, 3


int main(void) {

	// === simulation parameter ===
	const double        h = STEPSIZE;
	const double        T = SIMULATIONTIME;

	// === system parameter ===
	struct vector 	   x;						// state vector
	struct matrix 	   A;						// system matrix
	unsigned int  	   n = 0;    				// state dimension
	define_system(SYSTEMNUMBER, &x, &A, &n);

	// === Integration ===
	RuKu_integrate(&n, &A, &x, &T, &h);

	// clear memory
	free_mat(&A);
	free_vec(&x);

	return 0;
}
