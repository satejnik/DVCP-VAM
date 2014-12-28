//this is a file in the VAc dynamic model software package
//parent-routine: Separator.c
//subroutines involve: gamma_wilson_7.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//The file is used to complete a flash calculation assuming known P and T
//given a guess of liquid composition and partition coefficient, it calculates vapor composition and return:
//  1-sum(y), which is used to update partition coefficient
//  updated liquid composition
//The algorithm is based on public resource

#include <stdio.h>
#include <math.h>

void Beta_Separator(double error[], double feed_xL_out[], double beta[],
double ps[], double P[], double T[], double feed_xL[], double feed_comp[], 
double VIJ[][7], double aij[][7], double R, double T_ref) 
{
    int i;
    double sum1, sum2, K[7], feed_yV[7], g[7];

    gamma_wilson_7(g, T[0], feed_xL, VIJ, aij, R, T_ref);
    sum1=0.;
    sum2=0.;
    for (i=0;i<7;i++)
    {
    	K[i]=ps[i]*g[i]/P[0];
    	feed_xL_out[i]=feed_comp[i]/(1+(K[i]-1)*beta[0]);
    	feed_yV[i]=K[i]*feed_xL_out[i];
    	sum1+=feed_xL[i];
    	sum2+=feed_yV[i];
    };
    error[0] = sum1 - sum2;
}
