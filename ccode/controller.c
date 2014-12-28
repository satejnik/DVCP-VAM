//this is a file in the VAc dynamic model software package
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine provides a 'full-position' PID algorithm
//original author: Francisco Reyes-De Leon, Chemical Engineering Department, Lehigh University
//modified by Rong Chen
//Note: the controller gain, K, is dimentionless

//mex controller.c

#include <stdio.h>
#include <math.h>
//#include <mex.h>

void controller(double *manip_original, double *xi, double *hi_flag, double sp_original, double meas_scaled, double manip_original_in, double xi_in, int mode, double K, double Ti, double h, double LO_y, double HI_y, double LO_u, double HI_u, int action, int P_only, double rel_lo, double rel_hi, double hys, double hi_flag_in)
//manip_original(w/wo in): manipulated variable (u)
//xi(w/wo in): store the integration of error which is the second term in the controller formation
//sp_original: setpoint of the loop (y_y)
//meas_scaled: measurement (y) from the transmitter
//mode: 'automatic' is 1, 'manual' is 2, 'autotune' is 3
//K: scaled controller gain 
//Ti: controller in minute
//h: sampling time in minute 
//LO_y HI_y : measurement range
//LO_u HI_u: MV range
//action: +1 or -1 for controller gain sign
//P_only: proportional only controller =1 , else 0
//rel_lo rel_hi hys hi_flag: relay tuning parameters

{

    double sp_scaled, epsi, u, manip_scaled;

    sp_scaled = (sp_original - LO_y) /(HI_y - LO_y);
    epsi= sp_scaled - meas_scaled;
    if (mode == 1) //'Automatic' mode of operation
    {
        if (K != 0)
        {
            if (P_only==1)
            {
                u=K*action*epsi + xi_in;
                *xi=xi_in;
            }
            else
                u=K*action*(epsi+h*epsi/Ti) + xi_in;
        }
        else
            u=xi_in;
        if (u < 0) 
            manip_scaled = 0;
        else 
        {
            if (u > 1) 
                manip_scaled = 1;
            else
                manip_scaled = u;
        }
        if (P_only != 1)
        {
            if (K != 0)
                *xi= xi_in +h *K *action *epsi/Ti +(manip_scaled -u); // Antireset wind-up 
            else
                *xi= xi_in +h *action *epsi/Ti +(manip_scaled -u);   // Antireset wind-up 
        }
        *manip_original = manip_scaled * (HI_u - LO_u) + LO_u;
        *hi_flag=hi_flag_in;
        return;
    }
    else
    {   
        if (mode == 2) // 'Manual' mode
        {
            *manip_original=manip_original_in;
            *xi=xi_in;
            *hi_flag=hi_flag_in;
            return;
        }
        else  // 'Autotune' mode
        {
            if ((epsi > hys) && (hi_flag_in==0.0))
            {
                *manip_original = rel_hi;
                *hi_flag=1;
            }
            else
            {
                if ((epsi < -hys) && (hi_flag_in==1.0))
                {
                    *manip_original = rel_lo;
                    *hi_flag=0;
                }
            }
            return;
        }
    }
}

#ifdef mex_h
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double sp_original;
    double meas_scaled;
    double manip_original_in;
    double xi_in;
    int mode;
    double K;
    double Ti;
    double h;
    double LO_y;
    double HI_y;
    double LO_u;
    double HI_u;
    int action;
    int P_only;
    double rel_lo;
    double rel_hi;
    double hys;
    double hi_flag_in;

    double *manip_original;
    double *xi;
    double *hi_flag;
    
    if(nrhs!=18) 
        mexErrMsgTxt("inputs not correct");
    if(nlhs!=3) 
        mexErrMsgTxt("outputs not correct");
  
    sp_original = mxGetScalar(prhs[0]);
    meas_scaled = mxGetScalar(prhs[1]);
    manip_original_in = mxGetScalar(prhs[2]);
    xi_in = mxGetScalar(prhs[3]);
    mode = mxGetScalar(prhs[4]);
    K = mxGetScalar(prhs[5]);
    Ti = mxGetScalar(prhs[6]);
    h = mxGetScalar(prhs[7]);
    LO_y = mxGetScalar(prhs[8]);
    HI_y = mxGetScalar(prhs[9]);    
    LO_u = mxGetScalar(prhs[10]);
    HI_u = mxGetScalar(prhs[11]);
    action = mxGetScalar(prhs[12]);
    P_only = mxGetScalar(prhs[13]);
    rel_lo = mxGetScalar(prhs[14]);
    rel_hi = mxGetScalar(prhs[15]);
    hys = mxGetScalar(prhs[16]);
    hi_flag_in = mxGetScalar(prhs[17]);

    //-------------------------------------------
    plhs[0] = mxCreateDoubleScalar(0);
    manip_original = mxGetPr(plhs[0]);
    //-------------------------------------------
    plhs[1] = mxCreateDoubleScalar(0);
    xi = mxGetPr(plhs[1]);
    //-------------------------------------------
    plhs[2] = mxCreateDoubleScalar(0);
    hi_flag = mxGetPr(plhs[2]);

    controller(manip_original, xi, hi_flag, sp_original, meas_scaled, manip_original_in, xi_in, mode, K, Ti, h, LO_y, HI_y, LO_u, HI_u, action, P_only, rel_lo, rel_hi, hys, hi_flag_in);
}
#endif