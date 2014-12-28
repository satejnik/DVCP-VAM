//this is a file in the VAc dynamic model software package
//this is the main file for the vaporizer
//parent-routine: VAmodel.c
//subroutines involve: enthalpy_7.c gamma_wilson_7.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//if a user wants to build a DLL for the vaporizer only, follow the instruction below:  
//1.  Add this line to the head of this file: #include "mex.h"
//2.  Enable mexFunction(), which is the second routine included in this file (currently disabled)
//3.  Use the following line to generate a DLL file: vaporizer.dll 
//          mex vaporizer.c enthalpy_7.c gamma_wilson_7.c
//4.  The name of the generated function is "vaporizer()".  This function should be called in MATLAB.

//The vaporizer is modeled as a lumped system with VLE

#include <stdio.h>
#include <math.h>
#include "debug.h"
//#include "mex.h"

void Vaporizer(double *dstatedt, double *Level_Vaporizer, double *y_Vaporizer, double *T_Vaporizer, double *P_Vaporizer, 
                double states[], double F_VapVaporOut, double Q_Duty_Vaporizer, double F_GasIn,
                double y_GasIn[], double T_GasIn, double F_LiquidIn, double x_LiquidIn[],
                double T_LiquidIn, double Working_Level_Volume, 
                double VIJ[][7], double aij[][7], double MW[], double SpG[],double HVAP[], double HCAPLa[], double HCAPLb[],
                double HCAPVa[], double HCAPVb[], double A[], double B[], double C[], double R, double Dw, double T_ref) 

{

    //--------------------------------------------------------------------------
    //physical properties: VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref
    //--------------------------------------------------------------------------
    /*output:
        dstatedt: state derivatives
        Level_Vaporizer
        y_Vaporizer: vapor phase composition
        T_Vaporizer: vaporizer temperature (duplicated)
        P_Vaporizer: pressure
    --------------------------------------------------------------------------*/
    /*input:
        states: states
        F_GasIn: vaporizer gas feed flowrate
        y_GasIn: vaporizer gas feed composition
        T_GasIn: vaporizer gas feed temperature
        F_LiquidIn: vaporizer Liquid feed flowrate
        y_LiquidIn: vaporizer Liquid feed composition
        T_LiquidIn: vaporizer Liquid feed temperature
        F_VapVaporOut: vaporizer vapor outlet flowrate
        Q_Duty_Vaporizer: vpaorizer heat duty
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        Working_Level_Volume: liquid volume
    ------------------------------------------------------------------------*/

    //local variables
    //states
    double x[7], Volume, T[1];
         
    //intermediate variables
    double y[7], g[7], ps[7], P;
    double liquid_concentration, liquid_density;
    double X_LiquidIn[1][7], Y_GasIn[1][7], X[1][7], Y[1][7];    
    double H_GasIn[1], H_LiquidIn[1], HL[1], HV[1];
    
    //derivatives
    double x_dot[7], L_dot, M_dot, T_dot;
    
    //others    
    int i,j;
    double dummy[1], temp_T[1];
    double sum1, sum2, sum3, sum4;
    double temp_HV[1], temp_HL[1];  
#ifdef DEBUG
	debug("Vaporizer entered.\n");
#endif  
    /*------------------------------------------------------------------------------*/
    /*store states to x and M for convenience*/ 
    x[0] = states[0];
    x[1] = states[1];
    x[2] = states[2];
    x[4] = states[3];
    x[5] = states[4];
    x[6] = states[5];
    x[3]=1-(x[0]+x[1]+x[2]+x[4]+x[5]+x[6]);
    sum1=0.;
    sum2=0.;
    for (i=0;i<7;i++)
    {
        sum1+=x[i]*MW[i]/SpG[i];
        sum2+=x[i]*MW[i];
    };
    liquid_concentration=Dw/sum1;
    liquid_density=sum2*liquid_concentration;
    Volume=states[6]*Working_Level_Volume; /*m3*/
    T[0]=states[7];
    
    /*-----------------------------------------------------------------------------*/
    /*get enthalpy for input streams*/
    /*Gas*/
    for (j=0;j<7;j++)
        Y_GasIn[0][j] = y_GasIn[j];
    enthalpy_7(dummy, temp_HV, Y_GasIn, Y_GasIn, &T_GasIn, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    H_GasIn[0] = temp_HV[0];
    /*Liquid*/
    for (j=0;j<7;j++)
        X_LiquidIn[0][j] = x_LiquidIn[j];
    enthalpy_7(temp_HL, dummy, X_LiquidIn, X_LiquidIn, &T_LiquidIn, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    H_LiquidIn[0] = temp_HL[0];
    
    /*-----------------------------------------------------------------------------*/
    /*calculate y and liquid pressure*/
    gamma_wilson_7(g, T[0], x, VIJ, aij, R, T_ref);
    P=0.;
    for (i=0;i<7;i++)
    {
        ps[i]=exp(A[i]+B[i]/(T[0]+C[i]));
        P+=x[i]*ps[i]*g[i];
    };
    for (i=0;i<7;i++)
        y[i]=x[i]*ps[i]*g[i]/P;
    for (j=0;j<7;j++)
        X[0][j] = x[j];
    enthalpy_7(temp_HL, dummy, X, X, T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    HL[0] = temp_HL[0];
    for (j=0;j<7;j++)
        Y[0][j] = y[j];
    enthalpy_7(dummy, temp_HV, Y, Y, T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    HV[0] = temp_HV[0];
    
    /*-----------------------------------------------------------------------------*/
    /*get derivatives*/
    sum1=0.;
    sum2=0.;
    sum3=0.;
    sum4=0.;
    for (i=0;i<7;i++)
    {
        sum1+=y_GasIn[i]*MW[i];
        sum2+=x_LiquidIn[i]*MW[i];  
        sum3+=y[i]*MW[i];
        sum4+=x[i]*MW[i]*(HCAPLa[i]+HCAPLb[i]*T[0]);
    };
    M_dot=F_GasIn+F_LiquidIn-F_VapVaporOut; 
    L_dot=M_dot/liquid_concentration/Working_Level_Volume; /*volume percentage*/
    for (i=0;i<7;i++)
        x_dot[i]=(F_GasIn*y_GasIn[i]+F_LiquidIn*x_LiquidIn[i]-F_VapVaporOut*y[i]-x[i]*M_dot)/liquid_concentration/Volume;
    T_dot=(F_GasIn*(H_GasIn[0]-HL[0])+F_LiquidIn*(H_LiquidIn[0]-HL[0]) - F_VapVaporOut*(HV[0]-HL[0])+Q_Duty_Vaporizer)/(liquid_concentration*Volume)/sum4;
    
    /*-----------------------------------------------------------------------------*/
    //output
    *Level_Vaporizer=states[6];
    for (i=0;i<7;i++)
        y_Vaporizer[i]=y[i];
    *T_Vaporizer=T[0];
    *P_Vaporizer=P;

    /*-----------------------------------------------------------------------------*/
    dstatedt[0]=x_dot[0];
    dstatedt[1]=x_dot[1];
    dstatedt[2]=x_dot[2];
    dstatedt[3]=x_dot[4];
    dstatedt[4]=x_dot[5];
    dstatedt[5]=x_dot[6];
    dstatedt[6]=L_dot;
    dstatedt[7]=T_dot;
#ifdef DEBUG
	debug("Vaporizer left.\n");
#endif  
}

/*
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *states;
    double F_VapVaporOut;
    double Q_Duty_Vaporizer;
    double F_GasIn;
    double *y_GasIn;
    double T_GasIn;
    double F_LiquidIn;
    double *x_LiquidIn;
    double T_LiquidIn;
    double Working_Level_Volume; 
    double *VIJ;
    double *aij;
    double *MW;
    double *SpG;
    double *HVAP;
    double *HCAPLa;
    double *HCAPLb;
    double *HCAPVa;
    double *HCAPVb;
    double *A;
    double *B;
    double *C;
    double R;
    double Dw;
    double T_ref;
  
    int mrows;
    int ncols;
  
    double *dstatedt_Vap;
    double *Level_Vaporizer;
    double *y_Vaporizer;
    double *T_Vaporizer;
    double *P_Vaporizer;
    
    if(nrhs!=25) 
        mexErrMsgTxt("inputs not correct");
    if(nlhs!=5) 
        mexErrMsgTxt("outputs not correct");
  
    states = mxGetPr(prhs[0]);
    F_VapVaporOut = mxGetScalar(prhs[1]);
    Q_Duty_Vaporizer = mxGetScalar(prhs[2]);
    F_GasIn  = mxGetScalar(prhs[3]);
    y_GasIn  = mxGetPr(prhs[4]);
    T_GasIn  = mxGetScalar(prhs[5]);
    F_LiquidIn  = mxGetScalar(prhs[6]);
    x_LiquidIn  = mxGetPr(prhs[7]);
    T_LiquidIn  = mxGetScalar(prhs[8]);    
    Working_Level_Volume = mxGetScalar(prhs[9]);
    VIJ  = mxGetPr(prhs[10]);
    aij  = mxGetPr(prhs[11]);
    MW  = mxGetPr(prhs[12]);
    SpG  = mxGetPr(prhs[13]);
    HVAP  = mxGetPr(prhs[14]);
    HCAPLa  = mxGetPr(prhs[15]);
    HCAPLb  = mxGetPr(prhs[16]);
    HCAPVa  = mxGetPr(prhs[17]);
    HCAPVb  = mxGetPr(prhs[18]);
    A  = mxGetPr(prhs[19]);
    B  = mxGetPr(prhs[20]);
    C  = mxGetPr(prhs[21]);
    R  = mxGetScalar(prhs[22]);
    Dw  = mxGetScalar(prhs[23]);
    T_ref  = mxGetScalar(prhs[24]);

    //-------------------------------------------
    mrows = mxGetM(prhs[0]);
    ncols = mxGetN(prhs[0]);
    plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    dstatedt_Vap = mxGetPr(plhs[0]);
    //-------------------------------------------
    plhs[1] = mxCreateScalarDouble(0);
    Level_Vaporizer = mxGetPr(plhs[1]);
    //-------------------------------------------
    mrows = mxGetM(prhs[4]);
    ncols = mxGetN(prhs[4]);
    plhs[2] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    y_Vaporizer = mxGetPr(plhs[2]);
    //-------------------------------------------
    plhs[3] = mxCreateScalarDouble(0);
    T_Vaporizer = mxGetPr(plhs[3]);
    //-------------------------------------------
    plhs[4] = mxCreateScalarDouble(0);
    P_Vaporizer = mxGetPr(plhs[4]);

    Vaporizer(dstatedt_Vap,Level_Vaporizer,y_Vaporizer,T_Vaporizer,P_Vaporizer,
            states, F_VapVaporOut, Q_Duty_Vaporizer, F_GasIn, y_GasIn, T_GasIn, F_LiquidIn, x_LiquidIn, T_LiquidIn, 
            Working_Level_Volume, VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);
}
*/