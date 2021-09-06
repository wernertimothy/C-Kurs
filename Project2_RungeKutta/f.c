#include "matrixmath.h"

void f(struct matrix* pA, struct vector* px, struct vector* pdx) {
	mv_mult(pA, px, pdx); // dx = A*x
}
