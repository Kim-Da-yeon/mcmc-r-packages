/*
 * Unit-offset (1-based) array/matrix allocation helpers.
 * Original, self-contained implementation for this package: each routine
 * allocates a plain block with malloc and returns a pointer shifted down by
 * the requested lower bound, so the caller can index from that bound. The
 * free routines undo the shift before releasing. No third-party code.
 */
#include "nrutil.h"
#include <stdio.h>
#include <stdlib.h>

void nrerror(const char *msg)
{
    fprintf(stderr, "SAMC array-allocation error: %s\n", msg);
    exit(1);
}

/* ---------- one-dimensional vectors ---------- */
float *vector(int nl, int nh)
{
    float *p = (float *)malloc((size_t)(nh - nl + 1) * sizeof(float));
    if (!p) nrerror("out of memory in vector()");
    return p - nl;
}

int *ivector(int nl, int nh)
{
    int *p = (int *)malloc((size_t)(nh - nl + 1) * sizeof(int));
    if (!p) nrerror("out of memory in ivector()");
    return p - nl;
}

double *dvector(int nl, int nh)
{
    double *p = (double *)malloc((size_t)(nh - nl + 1) * sizeof(double));
    if (!p) nrerror("out of memory in dvector()");
    return p - nl;
}

/* ---------- two-dimensional matrices ----------
 * row-pointer array plus one malloc'd block per row, both unit-offset. */
float **matrix(int nrl, int nrh, int ncl, int nch)
{
    int i;
    float **m = (float **)malloc((size_t)(nrh - nrl + 1) * sizeof(float *));
    if (!m) nrerror("out of memory in matrix() (rows)");
    m -= nrl;
    for (i = nrl; i <= nrh; i++) {
        m[i] = (float *)malloc((size_t)(nch - ncl + 1) * sizeof(float));
        if (!m[i]) nrerror("out of memory in matrix() (cols)");
        m[i] -= ncl;
    }
    return m;
}

double **dmatrix(int nrl, int nrh, int ncl, int nch)
{
    int i;
    double **m = (double **)malloc((size_t)(nrh - nrl + 1) * sizeof(double *));
    if (!m) nrerror("out of memory in dmatrix() (rows)");
    m -= nrl;
    for (i = nrl; i <= nrh; i++) {
        m[i] = (double *)malloc((size_t)(nch - ncl + 1) * sizeof(double));
        if (!m[i]) nrerror("out of memory in dmatrix() (cols)");
        m[i] -= ncl;
    }
    return m;
}

int **imatrix(int nrl, int nrh, int ncl, int nch)
{
    int i;
    int **m = (int **)malloc((size_t)(nrh - nrl + 1) * sizeof(int *));
    if (!m) nrerror("out of memory in imatrix() (rows)");
    m -= nrl;
    for (i = nrl; i <= nrh; i++) {
        m[i] = (int *)malloc((size_t)(nch - ncl + 1) * sizeof(int));
        if (!m[i]) nrerror("out of memory in imatrix() (cols)");
        m[i] -= ncl;
    }
    return m;
}

/* view onto an existing matrix with a new offset (no new row storage) */
float **submatrix(float **a, int oldrl, int oldrh, int oldcl, int oldch, int newrl, int newcl)
{
    int i, j;
    float **m = (float **)malloc((size_t)(oldrh - oldrl + 1) * sizeof(float *));
    if (!m) nrerror("out of memory in submatrix()");
    m -= newrl;
    for (i = oldrl, j = newrl; i <= oldrh; i++, j++)
        m[j] = a[i] + oldcl - newcl;
    return m;
}

/* ---------- release routines (re-apply the offset) ---------- */
void free_vector(float *v, int nl, int nh)   { (void)nh; free(v + nl); }
void free_ivector(int *v, int nl, int nh)    { (void)nh; free(v + nl); }
void free_dvector(double *v, int nl, int nh) { (void)nh; free(v + nl); }

void free_matrix(float **m, int nrl, int nrh, int ncl, int nch)
{
    int i;
    (void)nch;
    for (i = nrh; i >= nrl; i--) free(m[i] + ncl);
    free(m + nrl);
}

void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch)
{
    int i;
    (void)nch;
    for (i = nrh; i >= nrl; i--) free(m[i] + ncl);
    free(m + nrl);
}

void free_imatrix(int **m, int nrl, int nrh, int ncl, int nch)
{
    int i;
    (void)nch;
    for (i = nrh; i >= nrl; i--) free(m[i] + ncl);
    free(m + nrl);
}

void free_submatrix(float **b, int nrl, int nrh, int ncl, int nch)
{
    (void)nrh; (void)ncl; (void)nch;
    free(b + nrl);
}

void free_convert_matrix(float **b, int nrl, int nrh, int ncl, int nch)
{
    (void)nrh; (void)ncl; (void)nch;
    free(b + nrl);
}
