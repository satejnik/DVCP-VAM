//this is a file in the VAc dynamic model software package
//this is the main file for the separator
//parent-routine: VAmodel.c
//subroutines involve: beta_separator.c enthalpy_7.c gamma_wilson_7.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//if a user wants to build a DLL for the separator only, follow the instruction below:  
//1.  Add this line to the head of this file: #include "mex.h"
//2.  Enable mexFunction(), which is the second routine included in this file (currently disabled)
//3.  Use the following line to generate a DLL file: separator.dll 
//          mex separator.c beta_separator.c enthalpy_7.c gamma_wilson_7.c
//4.  The name of the generated function is "separator()".  This function should be called in MATLAB.

//The separator is modeled as a partial condenser
//A static flash calculation is first carried out and then the two outlet streams are fed into the gas header and the liquid buffer tank respectively
//The most tricky thing in this model is that we assume the flash temperature is always 5 degC higher than the cooling jacket temperature

#include <stdio.h>
#include <math.h>
#include "debug.h"
//#include "mex.h"

void Separator(double *dstatedt, double states[], double Feed, double Feed_comp[], double Feed_T, double F_SepLiquidOUT,
    double T_SepCooler, double F_SepGasOUT, double Working_Level_Volume, double Total_Gas_Loop_Volume, double UA,
    double VIJ[][7], double aij[][7], double MW[], double SpG[],double HVAP[], double HCAPLa[], double HCAPLb[],
    double HCAPVa[], double HCAPVb[], double A[], double B[], double C[], double R, double Dw, double T_ref) 
{

    //--------------------------------------------------------------------------
    //physical properties: VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref
    //--------------------------------------------------------------------------
    /*output:
        dstatedt: state derivatives
        F_SepLiquidOUT_out: separator liqiud outlet flowrate (duplicated when no perfect control is selected)
    --------------------------------------------------------------------------*/
    /*input:
        states: states
        Feed: reactor feed flowrate
        Feed_comp: reactor feed composition
        Feed_T: reactor feed temperature
        F_SepLiquidOUT: liqiud outlet flowrate
        T_SepCooler: cooling jacket temperature
        F_SepGasOUT: gas outlet flowrate
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        Working_Level_Volume: liquid volume
        Total_Gas_Loop_Volume: gas header volume
        UA: Inverse of the Total Thermal Resistance
    ------------------------------------------------------------------------*/

    //local variables
    //states
    double xL[1][7], Volume, TL[1], yV[1][7], P[1], TV[1];
         
    //intermediate variables
    double liquid_concentration, liquid_density, Y_Feed_comp[1][7];
    double T[1]; //flash temperature
    double ps[7]; //partial pressure
    double Q, hl[1], hv[1], hl_feed[1], hv_feed[1];
    double VRT, error[1], Feed_V, Feed_L, Feed_xL[7], Feed_xL_i[7], Feed_yV[1][7], beta[1];  //flash calculation variables    

    //derivatives
    double dxdt[7], dydt[7], dMdt, dLdt, dTLdt, dPdt, dTVdt;
    
    //others    
    int i, j;
    double sum1, sum2;
    double dummy[1], temp_HL[1], temp_HV[1];
#ifdef DEBUG
	debug("Separator entered.\n");
#endif  
    /*get states*/
    sum1=0.;
    for (i=0;i<3;i++)
    {
    	xL[0][i]=states[i];
    	xL[0][i+4]=states[i+3];
    	sum1+=xL[0][i] + xL[0][i+4];
    };
    xL[0][3]=1-sum1;
    sum1=0.;
    sum2=0.;
    for (i=0;i<7;i++)
    {
    	sum1+=xL[0][i]*MW[i]/SpG[i];
    	sum2+=xL[0][i]*MW[i];
    };
    liquid_concentration=Dw/sum1;
    liquid_density=sum2*liquid_concentration;
    Volume=states[6]*Working_Level_Volume; /*m3*/
    TL[0]=states[7];
    enthalpy_7(temp_HL, dummy, xL, xL, TL, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    hl[0] = temp_HL[0];
    
    sum1=0.;
    for (i=0;i<3;i++)
    {
    	yV[0][i]=states[i+8];  
    	yV[0][i+4]=states[i+11];
    	sum1+=yV[0][i] + yV[0][i+4];
    };
    yV[0][3]=1-sum1;
    P[0]=states[14];
    TV[0]=states[15];
    enthalpy_7(dummy, temp_HV, yV, yV, TV, MW, HVAP, HCAPLa, HCAPLb, HCAPVa,HCAPVb);
    hv[0] = temp_HV[0];
    for (j=0;j<7;j++)
        Y_Feed_comp[0][j] = Feed_comp[j];
    enthalpy_7(dummy, temp_HV, Y_Feed_comp, Y_Feed_comp, &Feed_T, MW, HVAP, HCAPLa,	HCAPLb, HCAPVa, HCAPVb);
    hl_feed[0] = temp_HV[0];

    /*-----------------------------------------------------------------------------
    /*Do a flash calculation under known P and T*/
    T[0]=T_SepCooler+5;
    error[0]=1.0;
    for (i=0;i<7;i++)
    {
    	Feed_xL[i]=xL[0][i];
    	ps[i]=exp(A[i]+B[i]/(T[0]+C[i]));
    };
    beta[0]=F_SepGasOUT/Feed;
    while (fabs(error[0])>1e-10)
    { 
    	for (j=0;j<7;j++)
    		Feed_xL_i[j] = Feed_xL[j];
        Beta_Separator(error, Feed_xL, beta, ps, P, T, Feed_xL_i, Feed_comp, VIJ, aij, R, T_ref);
        beta[0]=beta[0]-0.1*error[0];
    }
    Feed_V=Feed*beta[0];
    Feed_L=Feed-Feed_V;
    for (i=0;i<7;i++)
    	Feed_yV[0][i]=(Feed*Feed_comp[i]-Feed_L*Feed_xL[i])/Feed_V;
    enthalpy_7(dummy, temp_HV, Feed_yV,Feed_yV, T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa,HCAPVb);
    hv_feed[0] = temp_HV[0];

    /*-----------------------------------------------------------------------------
    /*liquid dynamics*/
    sum1=0.;
    sum2=0.;
    for (i=0;i<7;i++)
    {
    	sum1+= Feed_xL[i]*MW[i];
    	sum2+= xL[0][i]*MW[i];
    };

    dMdt=Feed_L-F_SepLiquidOUT;
    dLdt=dMdt/liquid_concentration/Working_Level_Volume; /*volume percentage*/
    /*composition update*/
    sum1=0.;
    for (i=0;i<7;i++)
    {
    	dxdt[i]=(Feed_L*Feed_xL[i]-F_SepLiquidOUT*xL[0][i] - xL[0][i]*dMdt)/liquid_concentration/Volume;
    	sum1+=xL[0][i]*MW[i]*(HCAPLa[i]+HCAPLb[i]*TL[0]);
    };
    
    Q=UA*(T[0]-T_SepCooler);
    dTLdt=(Feed*(hl_feed[0]-hl[0])-Feed_V*(hv_feed[0]-hl[0]) - Q)/(liquid_concentration*Volume)/sum1;
    
    /*-----------------------------------------------------------------------------
    /*vapor dynamics*/ 
    VRT=Total_Gas_Loop_Volume/8.314/(TV[0]+273.15);
    dPdt=(Feed_V-F_SepGasOUT)/VRT*14.696/101.325;
    sum1=0.;
    for (i=0;i<7;i++)
    {
        dydt[i]=(Feed_V*Feed_yV[0][i]-F_SepGasOUT*yV[0][i]-VRT*yV[0][i]*dPdt)/VRT/(P[0]/14.696*101.325);
    	sum1+=yV[0][i]*MW[i]*(HCAPLa[i]+HCAPLb[i]*TV[0]);
    };
    dTVdt=Feed_V*(hv_feed[0]-hv[0])/(VRT*P[0])/sum1;
    
    /*-----------------------------------------------------------------------------*/
    //derivatives
    dstatedt[0] = dxdt[0];
    dstatedt[1] = dxdt[1];
    dstatedt[2] = dxdt[2];
    dstatedt[3] = dxdt[4];
    dstatedt[4] = dxdt[5];
    dstatedt[5] = dxdt[6];
    dstatedt[6] = dLdt;
    dstatedt[7] = dTLdt;
    dstatedt[8] = dydt[0];
    dstatedt[9] = dydt[1];
    dstatedt[10] = dydt[2];
    dstatedt[11] = dydt[4];
    dstatedt[12] = dydt[5];
    dstatedt[13] = dydt[6];
    dstatedt[14] = dPdt;
    dstatedt[15] = dTVdt;
#ifdef DEBUG
	debug("Separator left.\n");
#endif  
}

/*
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *states;
    double Feed;
    double *Feed_comp;
    double Feed_T;
    double F_SepLiquidOUT;
    double T_SepCooler;
    double F_SepGasOUT;
    double Working_Level_Volume;
    double Total_Gas_Loop_Volume;
    double UA;
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
  
    double *dstatedt_Sep;
    double *F_SepLiquidOUT_out;
    
    if(nrhs!=25) 
        mexErrMsgTxt("inputs not correct");
    if(nlhs!=1) 
        mexErrMsgTxt("outputs not correct");
  
    states = mxGetPr(prhs[0]);
    Feed = mxGetScalar(prhs[1]);
    Feed_comp  = mxGetPr(prhs[2]);
    Feed_T  = mxGetScalar(prhs[3]);
    F_SepLiquidOUT  = mxGetScalar(prhs[4]);
    T_SepCooler  = mxGetScalar(prhs[5]);
    F_SepGasOUT  = mxGetScalar(prhs[6]);
    Working_Level_Volume  = mxGetScalar(prhs[7]);
    Total_Gas_Loop_Volume = mxGetScalar(prhs[8]);
    UA  = mxGetScalar(prhs[9]);
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
    dstatedt_Sep = mxGetPr(plhs[0]);
  
    Separator(dstatedt_Sep,states,Feed,Feed_comp,Feed_T,
            F_SepLiquidOUT,T_SepCooler,F_SepGasOUT, Working_Level_Volume,Total_Gas_Loop_Volume,UA,
            VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);

}
*/