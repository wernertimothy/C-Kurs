#include "matrixmath.h"

void define_system(const unsigned int system_number, struct vector* px, struct matrix* pA, unsigned int* pn) {

	switch(system_number) {
	case 1:
		// state dimension
		*pn = 3;

		// initialize A = zeros(n,n) and x
		init_mat(pA, *pn, *pn);
		zero_matrix(pA);
		init_vec(px, *pn);

		// define matrix
		matrix_setval(pA, 0, 1,   1);
		matrix_setval(pA, 1, 2,   1);
		matrix_setval(pA, 2, 0,  -7);
		matrix_setval(pA, 2, 1, -13);
		matrix_setval(pA, 2, 2,  -5);

		// define initial state
		vector_setval(px, 0, 5);
		vector_setval(px, 1, 2);
		vector_setval(px, 2, 0);

		break;

	case 2:
		// state dimension
		*pn = 2;

		// initialize A = zeros(n,n) and x
		init_mat(pA, *pn, *pn);
		zero_matrix(pA);
		init_vec(px, *pn);

		matrix_setval(pA, 0, 0, -1);
		matrix_setval(pA, 1, 1,  1);

		vector_setval(px, 0, 2);
		vector_setval(px, 1, 2);

		break;

	case 3:
		// state dimension
		*pn = 4;

		// initialize A = zeros(n,n) and x
		init_mat(pA, *pn, *pn);
		zero_matrix(pA);
		init_vec(px, *pn);

		// define matrix
		matrix_setval(pA, 0, 1,   1);
		matrix_setval(pA, 1, 2,   1);
		matrix_setval(pA, 2, 3,   1);
		matrix_setval(pA, 3, 0, -24);
		matrix_setval(pA, 3, 1, -50);
		matrix_setval(pA, 3, 2, -35);
		matrix_setval(pA, 3, 3, -10);

		// define initial state
		vector_setval(px, 0, 1);
		vector_setval(px, 1, 1);
		vector_setval(px, 2, 1);
		vector_setval(px, 3, 1);

		break;

	} // cases

} // define_system
