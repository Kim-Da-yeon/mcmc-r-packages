#ifndef SAMC_ARRAYALLOC_H
#define SAMC_ARRAYALLOC_H

/*
 * Small self-contained helpers for 1-based (unit-offset) arrays and matrices.
 * Each allocation returns a pointer shifted down by its lower index, so callers
 * index with natural 1..n bounds; the matching free re-applies the shift.
 * Original implementation for this package (plain malloc wrappers).
 */

float  *vector (int nl, int nh);
int    *ivector(int nl, int nh);
double *dvector(int nl, int nh);
float  **matrix (int nrl, int nrh, int ncl, int nch);
double **dmatrix(int nrl, int nrh, int ncl, int nch);
int    **imatrix(int nrl, int nrh, int ncl, int nch);
float  **submatrix(float **a, int oldrl, int oldrh, int oldcl, int oldch, int newrl, int newcl);

void free_vector (float  *v, int nl, int nh);
void free_ivector(int    *v, int nl, int nh);
void free_dvector(double *v, int nl, int nh);
void free_matrix (float  **m, int nrl, int nrh, int ncl, int nch);
void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch);
void free_imatrix(int    **m, int nrl, int nrh, int ncl, int nch);
void free_submatrix(float **b, int nrl, int nrh, int ncl, int nch);
void free_convert_matrix(float **b, int nrl, int nrh, int ncl, int nch);

void nrerror(const char *msg);

#endif
