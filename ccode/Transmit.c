//this is a file in the VAc dynamic model software package
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine provides a transmitter algorithm
//original author: Francisco Reyes-De Leon, Chemical Engineering Department, Lehigh University
//modified by Rong Chen

//mex transmit.c

#include <stdio.h>
#include <math.h>
//#include <mex.h>

void Transmit(double *x_out, double *y_out, double y_original, double x[], double y[], double ded, double tau, int Nded, int Nlag, double h, double LO, double HI)
//y_original: measurement
//x: storage spaces for deadtime
//y: storage spaces for lag time
//ded: should be "transmitter_deadtime" defined above
//tau: should be "transmitter_lag" defined above
//Nded: measurement deadtime storage, which should be 1+transmitter_deadtime*transmitter_sampling_frequency
//Ntau: measurement time constant storage, which should be 1 all the time
//h: transmitter sampling time
//LO HI: measurement range

{
    int i;
    double dy;

    if (ded >= 1e-4)
    {
        for (i=Nded-1;i>=1;i--)
        {
            x_out[i]= x[i-1];
        }
    }
    x_out[0]= (y_original -LO)/(HI -LO);
    if (x_out[0] <0) 
        x_out[0]= 0;
    else 
    {
        if (x_out[0] >1) 
            x_out[0]= 1;
    }
    if (tau > 1e-4)         
    {
        for (i=0;i<Nlag;i++)
        {
            if (i==0)
                dy = (x_out[Nded-1] - y[i]) /tau;
            else
                dy= (y[i-1] -y[i]) /tau;
            y_out[i]= y[i] + dy *h;
        }
    }
    else //tau==0
    {    
        for (i=0;i<Nlag;i++)
            y_out[i] = x_out[Nded-1];
    }
}

#ifdef mex_h

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double y_original;
    double *x;
    double *y;
    double ded;
    double tau;
    int Nded;
    int Nlag;
    double h;
    double LO;
    double HI;  

    int mrows;
    int ncols;
  
    double *x_out;
    double *y_out;

    if(nrhs!=10) 
        mexErrMsgTxt("inputs not correct");
    if(nlhs!=2) 
        mexErrMsgTxt("outputs not correct");
  
    y_original = mxGetScalar(prhs[0]);
    x = mxGetPr(prhs[1]);
    y = mxGetPr(prhs[2]);
    ded = mxGetScalar(prhs[3]);
    tau = mxGetScalar(prhs[4]);
    Nded = mxGetScalar(prhs[5]);
    Nlag = mxGetScalar(prhs[6]);
    h = mxGetScalar(prhs[7]);
    LO = mxGetScalar(prhs[8]);
    HI = mxGetScalar(prhs[9]);

    //-------------------------------------------
    mrows = mxGetM(prhs[1]);
    ncols = mxGetN(prhs[1]);
    plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    x_out = mxGetPr(plhs[0]);
    //-------------------------------------------
    mrows = mxGetM(prhs[2]);
    ncols = mxGetN(prhs[2]);
    plhs[1] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    y_out = mxGetPr(plhs[1]);

    Transmit(x_out, y_out, y_original, x, y, ded, tau, Nded, Nlag, h, LO, HI);
}

#endif
