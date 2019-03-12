/*
 * CausalButter.c
 *
 *  Created on: Jun 12, 2018
 *      Author: junqing
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct ButterHighPassData
{
	double n,f,s,r,a,a2,*A,*d1,*d2,*w0,*w1,*w2;
};

void initHPFilter(struct ButterHighPassData *filterData, int filter_order, int hpf, int sps)
{
	filterData->n = filter_order/2;
	filterData->f = hpf;
	filterData->s = sps;
	filterData->a = tan(M_PI*filterData->f/filterData->s);
	filterData->a2 = filterData->a*filterData->a;
	filterData->A = (double *)malloc(filterData->n*sizeof(double));
	filterData->d1 = (double *)malloc(filterData->n*sizeof(double));
	filterData->d2 = (double *)malloc(filterData->n*sizeof(double));
	filterData->w0 = (double *)calloc(filterData->n, sizeof(double));
	filterData->w1 = (double *)calloc(filterData->n, sizeof(double));
	filterData->w2 = (double *)calloc(filterData->n, sizeof(double));
	filterData->r = 0;

	for(int i = 0; i <filterData->n ; i++)
	{
		filterData->r = sin(M_PI*(2.0*i + 1.0)/(4.0*filterData->n));
		filterData->s = filterData->a2 + 2.0*filterData->a*filterData->r + 1.0;
		filterData->A[i] = 1.0/filterData->s;
		filterData->d1[i] = 2.0*(1-filterData->a2)/filterData->s;
		filterData->d2[i] = -(filterData->a2 - 2.0*filterData->a*filterData->r + 1.0)/filterData->s;
	}

	printf("Test HP a %f, a2 %f, A[0] %f, r %f, D2 %f\n",filterData->a ,filterData->a2,filterData->A[0], filterData->r,filterData->d2[0]);
}


double runHPFilter(struct ButterHighPassData *filter, double x)
{
	for(int i=0; i<filter->n; ++i)
	{
		filter->w0[i] = filter->d1[i]*filter->w1[i] + filter->d2[i]*filter->w2[i] + x;
		x = filter->A[i]*(filter->w0[i] - 2.0*filter->w1[i] + filter->w2[i]);
		filter->w2[i] = filter->w1[i];
		filter->w1[i] = filter->w0[i];
	}
	return x;
}

