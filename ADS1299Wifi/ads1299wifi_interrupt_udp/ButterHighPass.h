/*
 * ButterHighPass.h
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

void initHPFilter(struct ButterHighPassData *filterData, int filter_order, int hpf, int sps);

double runHPFilter(struct ButterHighPassData *filter, double x);
