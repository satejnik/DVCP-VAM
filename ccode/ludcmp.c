//Two files in the VAc dynamic model software package
//parent-routine: column.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is to do the LU decomposition calculation
//The matrix A is now a vector A of size N^2  
//The variable N does not refer to the number of grid points along the x or y axis
//It instead refers to the total number of grid points in the problem
//So if you split your 2-D area into a 11x11 grid, N would equal 121.  
//source code is from http://www.cen.uiuc.edu/~me310/home.html
//Courtesy: Travis Grove

#include <math.h>
#include <mex.h>
#include "debug.h"

Ludcmp(int N, double A[], int Indx[])
{
    int i, j, k, D, Imax;
    double Tiny, Dum, sum, AAmax;
    double *VV;
#ifdef DEBUG
	debug("Ludcmp entered.\n");
#endif
#ifdef mex_h
	VV = (double *)mxCalloc(N, sizeof(double));
#else
    VV = (double *)calloc(N, sizeof(double));
#endif
    Tiny=1E-20;
    D=1;
    for (i=0; i<N; i++)
    {
        AAmax=0;
        for(j=0; j<N; j++)
        {
            if(fabs(A[i*N+j])>AAmax)
    	    {
        		AAmax=fabs(A[i*N+j]);
    	    }
        }
        if(AAmax==0)
        {
            printf("Singular Matrix \n");
        }
        VV[i]=(double) 1/AAmax;
    }
    for(j=0; j<N; j++)
    {
        if(j>0)
        {
            for(i=0; i<j; i++)
            {
                sum=A[i*N+j];
                if(i>0)
                {
                     for(k=0; k<i; k++)
                     {
                         sum=sum-A[i*N+k]*A[k*N+j];
                     }
                     A[i*N+j]=sum;
                }
            }
        }
        AAmax=0;
        for(i=j; i<N; i++)
        {
            sum=A[i*N+j];
            if(j>0)
            {
                for(k=0; k<j; k++)
                {
                    sum=sum-A[i*N+k]*A[k*N+j];
                }
                A[i*N+j]=sum;
            }
            Dum=VV[i]*fabs(sum);
            if(Dum>=AAmax)
            {
                Imax=i;
                AAmax=Dum;
            }
        }
        if(j!=Imax)
        {
            for(k=0; k<N; k++)
            {
                 Dum=A[Imax*N+k];
                 A[Imax*N+k]=A[j*N+k];
                 A[j*N+k]=Dum;
             }
             D=-D;
             VV[Imax]=VV[j];
        }
        Indx[j]=Imax;

        if(j!=N-1)
        {
            if(A[j*N+j]==0)
            {
                A[j*N+j]=Tiny;
            }
            Dum=(double)1/A[j*N+j];
    	    for(i=j+1; i<N; i++)
            {
                A[i*N+j]=A[i*N+j]*Dum;
            }
        }
    }
    if(A[N*N-1]==0)
    {
        A[N*N-1]=Tiny;
    }
#ifdef mex_h

	mxFree(VV);
#else 
    free(VV);
#endif
#ifdef DEBUG
	debug("Ludcmp left.\n");
#endif
}



void Lubksb(int N, double A[], int Indx[], double B[])
{
    
    int ii,i,j,ll;
    double sum;
#ifdef DEBUG
	debug("Lubksb entered.\n");
#endif
    ii=-1;

    for(i=0; i<N; i++)
    {
        ll=Indx[i];
        sum=B[ll];
        B[ll]=B[i];
        if(ii!=-1)
        {
                for(j=ii; j<i; j++)
                {
                    sum=sum-A[i*N+j]*B[j];
                }
        }
        else
        {
                if(sum!=0)
                {
                    ii=i;
                }
        }
        B[i]=sum;
    }

    for(i=N-1; i>=0; i--)
    {
        sum=B[i];
        if(i<N-1)
        {
            for(j=i+1; j<N; j++)
            {
                sum=sum-A[i*N+j]*B[j];
            }
        }
        B[i]=sum/A[i*N+i];
    }
#ifdef DEBUG
	debug("Lubksb left.\n");
#endif
}

