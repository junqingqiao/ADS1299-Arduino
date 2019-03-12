
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct ButterBandStopData                                                                               
{                                                                                                       
    double s, f1, f2, a, a2, b, b2, r, n; 
    double *A, *d1, *d2, *d3, *d4, *w0, *w1, *w2, *w3, *w4;                                             
};  


void initBSFilter(struct ButterBandStopData *d, int filter_order, int f1, int f2, int sps);
double runBSFilter(struct ButterBandStopData *d, double x); 
