#ifndef MATRIXMATH_H
#define MATRIXMATH_H

//! Funktion erfolgreich abgeschlossen
#define SUCCESS 0
//! Der Funktion wurde ein Nullpointer übergeben
#define NULL_POINTER_ARGUMENT 1
//! Für den korrekten Ablauf der Funktion stand nicht genügend Speicher zur Verfügung
#define OUTOFMEMORY 2
//! Bei einer Matrix-/Vektoroperation wurden Matrizen/Vektoren unpassender Größe miteinander verrechnet
#define DIMENSION_MISSMATCH 3
//! Es wurde auf ein Matrix-/Vektorelement zugegriffen, dass sich außerhalb des Definitionsbereiches der Matrix befand
#define OUTOFRANGE 4


/*! Matrixstruktur
	 * Enthält einen Pointer auf den Speicherbereich sowie die Dimensionen der Matrix
	 */
struct matrix {

	//! Doppelpointer auf Matrizenelemente
	double ** ppdData;
	//! Anzahl der Spalten
	int icol;
	//! Anzahl der Zeilen
	int irow;

};

/* Vektorstruktur
 * Enthält Pointer auf Speicherbereich sowie Dimension des Vektors
 */
struct vector {
	double* pdData;
	int irow;
};

int mprint(struct matrix * pmA);
int vprint(struct vector * pvv);
int mm_mult(struct matrix * pmA, struct matrix * pmB, struct matrix* pmres);
int mm_add(struct matrix * pmA, struct matrix * pmB, struct matrix * pmres);
int mv_mult(struct matrix* pmA, struct vector * pvv, struct vector * pvres);
int vv_add(struct vector* pvv, struct vector* pvx, struct vector* pvres);
int m_scal(struct matrix * pmA, double dscal, struct matrix * pmres);
int v_scal(struct vector* pvv, double dscal, struct vector* pvres);
double** mat_malloc(int iz, int is);
double* vec_malloc(int iz);
int v_copy(struct vector* pvx, struct vector* pvy);
int m_copy(struct matrix * pmA, struct matrix * pmB);
int init_mat(struct matrix * pmA, int iz, int is);
int init_vec(struct vector * pvv, int iz);
int free_mat(struct matrix * pmA);
int free_vec(struct vector * pvv);

struct vector * get_vec(int iz);
struct matrix * get_mat(int iz, int is);
int zero_matrix(struct matrix * pmA);
int zero_vector(struct vector * pvv);
int matrix_setval(struct matrix * pmA, int izeile, int ispalte, double neuer_wert);
int vector_setval(struct vector * pvv, int izeile, double neuer_wert);
int matrix_getval(struct matrix * pmA, int izeile, int ispalte, double *ausgelesener_wert);
int vector_getval(struct vector * pvv, int izeile, double *ausgelesener_wert);


#endif
