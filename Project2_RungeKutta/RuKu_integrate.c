#include <stdio.h>
#include "matrixmath.h"
#include "f.h"

void RuKu_integrate(unsigned int* pn, struct matrix* pA, struct vector* px, const double* T, const double* h) {

	const unsigned int N = (*T)/(*h); // be aware of implicit type conversion !

	// init k1 to k4
	struct vector k1, k2, k3, k4;
	init_vec(&k1, *pn);
	init_vec(&k2, *pn);
	init_vec(&k3, *pn);
	init_vec(&k4, *pn);

	// init dummy vector for results
	struct vector p;
	init_vec(&p, *pn);

	// log initial state
	printf("--- t=%.2f ---\n", 0.0);
	vprint(px); // initial state

	// this loop will perform N RUKU4 steps
	for(unsigned int k = 1; k <= N; k++) {

		// calculate k1, k2, k3, k4
		// === k1 ===
		f(pA, px, &k1);          	// k1 = f(x)
		// === k2 ===
		v_scal(&k1, (*h)/2, &p);    // p  = h/2*k1
		vv_add(px, &p, &p);      	// p  = x + h/2*k1
		f(pA, &p, &k2);          	// k2 = f(p)
		// === k3 ===
		v_scal(&k2, (*h)/2, &p);    // p  = h/2*k2
		vv_add(px, &p, &p);      	// p  = x + h/2*k2
		f(pA, &p, &k3);          	// k3 = f(p)
		// === k4 ===
		v_scal(&k3, (*h), &p);      // p  = h*k2
		vv_add(px, &p, &p);      	// p  = x + h*k3
		f(pA, &p, &k4);          	// k4 = f(p)

		// calculate RuKu4 procedure
		v_scal(&k2, 2, &k2);	 	// k2 = 2*k2
		v_scal(&k3, 2, &k3);	 	// k3 = 2*k3
		vv_add(&k3, &k4, &p);    	// p  = 2k3 + k4
		vv_add(&k2, &p, &p);     	// p  = 2k2 + 2k3 + k4
		vv_add(&k1, &p, &p);     	// p  = k1 + 2k2 + 2k3 + k4
		v_scal(&p, (*h)/6, &p);	 	// p  = h*(k1 + 2k2 + 2k3 + k4)/6

		// calculate x_{k+1}
		vv_add(px, &p, px);      	// x+ = x + h*(k1 + 2k2 + 2k3 + k4)/6

		// log state
		printf("--- t=%.2f ---\n", k*(*h));
		vprint(px);


	} // for k in range(0, N)

	// free memory
	free_vec(&k1);
	free_vec(&k2);
	free_vec(&k3);
	free_vec(&k4);
	free_vec(&p);

} // Ruku_integrate
