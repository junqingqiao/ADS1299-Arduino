/*
 * BUGMAN
 * Reference http://www.exstrom.com/journal/sigproc
 * BandStop Butter
 * Created on: Aug 7, 2018
 *
 */

#include "ButterBandStop.h"


void initBSFilter(struct ButterBandStopData *d, int filter_order, int f1, int f2, int sps)
{
    d->n = filter_order/4;
    d->f1 = f1;
    d->f2 = f2;
    d->s = sps;
    d->a = cos(M_PI*(d->f1+d->f2)/d->s)/cos(M_PI*(d->f1-d->f2)/d->s);
    d->a2 = d->a*d->a;
    d->b = tan(M_PI*(d->f1-d->f2)/d->s);
    d->b2 = d->b*d->b;
    d->A = (double *)malloc(d->n*sizeof(double));
    d->d1 = (double *)malloc(d->n*sizeof(double));
    d->d2 = (double *)malloc(d->n*sizeof(double));
    d->d3 = (double *)malloc(d->n*sizeof(double));
    d->d4 = (double *)malloc(d->n*sizeof(double));
    d->w0 = (double *)calloc(d->n, sizeof(double));
    d->w1 = (double *)calloc(d->n, sizeof(double));
    d->w2 = (double *)calloc(d->n, sizeof(double));
    d->w3 = (double *)calloc(d->n, sizeof(double));
    d->w4 = (double *)calloc(d->n, sizeof(double));

    for(int i = 0; i<d->n ;i++)
    {
        d->r = sin(M_PI*(2.0*i+1.0)/(4.0*d->n));
        d->s = d->b2 + 2.0*d->b*d->r + 1.0;
        d->A[i] = 1.0/d->s;
        d->d1[i] = 4.0*d->a*(1.0+d->b*d->r)/d->s;
        d->d2[i] = 2.0*(d->b2-2.0*d->a2-1.0)/d->s;
        d->d3[i] = 4.0*d->a*(1.0-d->b*d->r)/d->s;
        d->d4[i] = -(d->b2 - 2.0*d->b*d->r + 1.0)/d->s;
    }
    d->r = 4.0*d->a;
    d->s = 4.0*d->a2 + 2.0;
    printf("Test BS a %f, a2 %f, A[0] %f, r %f, D2 %f\n",d->a ,d->a2,d->A[0], d->r, d->d2[0]);
}

double runBSFilter(struct ButterBandStopData *d, double x)
{
    for(int i=0; i<d->n; ++i){
        d->w0[i] = d->d1[i]*d->w1[i] + d->d2[i]*d->w2[i]+ d->d3[i]*d->w3[i]+ d->d4[i]*d->w4[i] + x;
        x = d->A[i]*(d->w0[i] - d->r*d->w1[i] + d->s*d->w2[i]- d->r*d->w3[i] + d->w4[i]);
        d->w4[i] = d->w3[i];
        d->w3[i] = d->w2[i];
        d->w2[i] = d->w1[i];
        d->w1[i] = d->w0[i];
    }
    return x;
}



