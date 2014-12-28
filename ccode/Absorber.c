//this is a file in the VAc dynamic model software package
//this is the main file for the absorber
//parent-routine: VAmodel.c
//subroutines involve: enthalpy_7.c gamma_wilson_7.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//if a user wants to build a DLL for the absorber only, follow the instruction below:  
//1.  Add this line to the head of this file: #include "mex.h"
//2.  Enable mexFunction(), which is the second routine included in this file (currently disabled)
//3.  Use the following line to generate a DLL file: absorber.dll 
//          mex absorber.c enthalpy_7.c gamma_wilson_7.c
//4.  The name of the generated function is "absorber()".  This function should be called in MATLAB.

//The absorber is modeled by a eight-trayed column with a base tank
//The most tricky part is to use a rate-based method to calculate the mass and heat transfered on each tray
//A VLE is assumed between the liquid phase and an interface vapor phase
//Mass and heat are transferred between the vapor phase and the interface vapor phase using constant transfer coefficients
//The base tank is a basic liquid buffer tank, modeled by using mass and energy balances

#include <stdio.h>
#include <math.h>
#include "debug.h"
//#include "mex.h"

void Absorber(double *dstatedt, double *dstatedt_other, double *Level, double *TV, double *TL,
    double *F_AbsGasOut, double *y_AbsGasOut,
    double *T_AbsGasOut, double *x_AbsLiquidOut, double *T_AbsLiquidOut, 
    double states[], double Pin, double F_AbsLiquidOut, double F_AbsGasIn,
    double y_AbsGasIn[], double T_AbsGasIn, double F_AbsLiquidIn,
    double x_AbsLiquidIn[], double T_AbsLiquidIn, double F_Circulation, double Q_Circulation, double Q_Scrub,
    int NT, int NF, double Working_Level_Volume, double M0[], double L0[], double hydtau,
    double Nmt[], double Qmt[], double T_Circulation[], double T_Scrub[], 
    double VIJ[][7], double aij[][7], double MW[], double SpG[], double HVAP[],
    double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[],
    double A[], double B[], double C[], double R, double Dw, double T_ref)
{

    //--------------------------------------------------------------------------
    //physical properties: VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref
    //--------------------------------------------------------------------------
    /*output:
        dstatedt: state derivatives
        dstatedt_other: temperature state derivatives
        Level: absorber level
        TV: absorber vapor outlet temperature 
        TL: absorber liquid outlet temperature 
        Q_Circulation_out: circulation cooler duty (duplicated when no perfect control is selected)
        Q_Scrub_out: scrub cooler duty (duplicated when no perfect control is selected)
        F_AbsGasOut: absorber vapor outlet flowrate
        y_AbsGasOut: absorber vapor outlet composition
        T_AbsGasOut: absorber vapor outlet temperature
        F_AbsLiquidOut_out: absorber liquid outlet flowrate
        x_AbsLiquidOut: absorber liquid outlet composition
        T_AbsLiquidOut: absorber liquid outlet temperature 
    --------------------------------------------------------------------------*/
    /*input:
        states: states
        F_AbsLiquidOut: absorber bottom liquid outlet flowrate
        F_AbsGasIn: abosrber gas inlet flowrate
        y_AbsGasIn: abosrber gas inlet composition
        T_AbsGasIn: abosrber gas inlet temperature
        F_AbsLiquidIn: abosrber liquid inlet flowrate (the scrub stream)
        x_AbsLiquidIn: abosrber liquid inlet composition
        T_AbsLiquidIn: abosrber liquid inlet temperature
        F_Circulation: abosrber circulation stream flowrate
        Q_Circulation: abosrber circulation cooler duty
        Q_Scrub:  abosrber liquid inlet cooler duty
        T_circulation: absorber circulation stream temperature
        T_Scrub: absorber scrub stream (liquid inlet stream) temperature
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        Pin: absorber pressure
        NT: tray number
        NF: circulation stream feed tray
        Working_Level_Volume: absorber bottom volume
        M0[]: tray holdup constant
        L0[]: liquid flowrate constant
        hydtau: hydraulic constant
        Nmt: absorber mass transfer coefficients
        Qmt: absorber heat transfer coefficients    
    ------------------------------------------------------------------------*/

    //local variables
    //states
    double x[8][7], M[8];    
    double x_base[1][7], Volume_base, T_base[1];    
    double x_Circulation[1][7];  //same as x_base
    //intermediate variables
    double y[8][7];
    double ps[8][7]; //partial pressure
    double g[8][7]; //gamma
    double y_interface[8][7];
    double z[8][7]; //mass transfer flow composition
    double minimum[8][7]; //real mass transfer flow without direction
    double N[8][7]; //real mass transfer flow with direction
    double P[8], FL[8], FV[8], Q[8];
    double HL[8], HV[8], H_N[8];
    double HV_AbsGasIn[1], HL_AbsLiquidIn[1], HL_Scrub[1], HL_base[1], HL_Circulation[1];
    double liquid_concentration, liquid_density;
    //Feed
    double Y_AbsGasIn[1][7], X_AbsLiquidIn[1][7];
    //derivatives
    double dMdt[8], dxdt[8][7], dTdt[8], dxbasedt[7], dMbasedt, dLbasedt, dTbasedt;
    //others    
    int i, j, k;
    double sum1=0., sum2=0., Mol_fract=0., dummy[1] = {0.}; 
    double temp_X[1][7], temp_Y[1][7], temp_T[1], temp_HL[1], temp_HV[1];
    double AAA, BBB, CCC, TTT, gg[7], sign[7], sumN[8];
#ifdef DEBUG
	debug("Absorber entered.\n");
#endif  
    //-------------------------------------------------------------------------------
    //Initialization
    for (i=0;i<NT;i++)
    {
    	x[i][0]=states[i];
    	x[i][1]=states[i+NT];
    	x[i][2]=states[i+2*NT];
    	x[i][4]=states[i+3*NT];
    	x[i][5]=states[i+4*NT];
    	x[i][6]=states[i+5*NT];
    	x[i][3]=1-(x[i][0]+x[i][1]+x[i][2]+x[i][4]+x[i][5]+x[i][6]);
    	M[i]=states[i+6*NT];
    	TL[i]=states[i+7*NT];
    };
    x_base[0][0]=states[8*NT]; 
    x_base[0][1]=states[8*NT+1];
    x_base[0][2]=states[8*NT+2];
    for (i=0;i<6;i++)
    	Mol_fract+=states[i+8*NT];
    x_base[0][3]= 1- Mol_fract;
    x_base[0][4]=states[8*NT+3];
    x_base[0][5]=states[8*NT+4];
    x_base[0][6]=states[8*NT+5];
    sum1=0.;
    sum2=0.;
    for (i=0;i<7; i++)
    {
    	sum1+=x_base[0][i]*MW[i]/SpG[i];
    	sum2+=x_base[0][i]*MW[i];
    };
    liquid_concentration=Dw/sum1;
    liquid_density=sum2*liquid_concentration;
    Volume_base=states[8*NT+6]*Working_Level_Volume; /*moles.*/
    T_base[0]=states[8*NT+7];
    for (i=0;i<7;i++)
    	x_Circulation[0][i]=x_base[0][i];
    for (i=0;i<8;i++)
    	P[i]=Pin;

    //-------------------------------------------------------------------------------
    //input stream property calculation
    //gas
    for (j=0;j<7;j++)
        Y_AbsGasIn[0][j] = y_AbsGasIn[j];
    enthalpy_7(dummy, HV_AbsGasIn, Y_AbsGasIn, Y_AbsGasIn, &T_AbsGasIn, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    //circulation stream: if perfect control is selected, the circulation cooler duty is calculated based on the setpoint 25 degC
    //scrub stream: if perfect control is selected, the scrub cooler duty is calculated based on the setpoint 25 degC
    //circulation
   	enthalpy_7(HL_base, dummy, x_base, x_base, T_base, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
   	CCC=-(F_Circulation*HL_base[0]-Q_Circulation)/F_Circulation;
   	AAA=0.;
   	BBB=0.;
   	for (i=0;i<7;i++)
    {
  		BBB+=HCAPLa[i]*MW[i]*x_Circulation[0][i];
   		AAA+=0.5*HCAPLb[i]*MW[i]*x_Circulation[0][i];
   	};
   	if ((BBB*BBB-4*AAA*CCC)<0)
	    printf("error on circulation temp");
   	//calculate the temperature derivative, tau=2 minute
   	dstatedt_other[2]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA)-T_Circulation[0])/2;
   	enthalpy_7(HL_Circulation, dummy,x_Circulation, x_Circulation,	T_Circulation, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    //scrub
    for (j=0;j<7;j++)
        X_AbsLiquidIn[0][j] = x_AbsLiquidIn[j];
    temp_T[0]=T_AbsLiquidIn;
  	enthalpy_7(HL_AbsLiquidIn, dummy, X_AbsLiquidIn, X_AbsLiquidIn, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
   	CCC=-(F_AbsLiquidIn*HL_AbsLiquidIn[0]-Q_Scrub)/F_AbsLiquidIn;
   	AAA=0.;
   	BBB=0.;
   	for (i=0;i<7;i++)
    {
	    BBB+=HCAPLa[i]*MW[i]*x_AbsLiquidIn[i];
   		AAA+=0.5*HCAPLb[i]*MW[i]*x_AbsLiquidIn[i];
   	};
   	if ((BBB*BBB-4*AAA*CCC)<0)	  
   		printf("error on scrub temp");
   	//calculate the temperature derivative, tau=2 minute
   	dstatedt_other[3]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA)-T_Scrub[0])/2;
   	enthalpy_7(HL_Scrub, dummy, X_AbsLiquidIn, X_AbsLiquidIn, T_Scrub, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);

    /*--------------------------------------------------------------------------*/
    //column dynamics calculation
    /*Liquid flows assuming linearized tray hydraulics with time constant hydtau*/
    for (i=0;i<NT;i++)
    {
    	FL[i] = L0[i] + (M[i]-M0[i])/hydtau;
    	for (j=0;j<7;j++)
	    {
    		ps[i][j]=exp(A[j]+B[j]/(TL[i]+C[j]));
		    temp_X[0][j]=x[i][j];
    	}; 
		TTT=TL[i];
		gamma_wilson_7(gg, TTT, temp_X[0], VIJ, aij, R, T_ref);
    	for (j=0;j<7;j++)
    	{
    		g[i][j]=gg[j];
    		y_interface[i][j]=ps[i][j]*x[i][j]*g[i][j]/P[i];
    	};

      	if (i==0)
    	{
    	    //bottom tray
    	    sumN[i]=0.;
    		for (j=0;j<7;j++)
    		{
    			if (y_AbsGasIn[j]==y_interface[i][j])
				    sign[j] = 0.;
    			else
				    sign[j] =(y_AbsGasIn[j]-y_interface[i][j]) / fabs(y_AbsGasIn[j]-y_interface[i][j]);
       			if(fabs(Nmt[i]*(y_AbsGasIn[j]-y_interface[i][j])) > 0.5*F_AbsGasIn*y_AbsGasIn[j])
       				minimum[i][j]=0.5*F_AbsGasIn*y_AbsGasIn[j];
       			else
       				minimum[i][j]= fabs(Nmt[i]*(y_AbsGasIn[j]-y_interface[i][j]));
       			N[i][j]=sign[j]*minimum[i][j];
       			sumN[i]+=N[i][j];
    		};
        	FV[i]=F_AbsGasIn-sumN[i];
    		for (j=0;j<7;j++)
    		{
        		y[i][j]=(F_AbsGasIn*y_AbsGasIn[j]-N[i][j])/FV[i];
        		z[i][j]=N[i][j]/sumN[i];
    		};
    		Q[i]=Qmt[i]*(T_AbsGasIn-TL[i])*9/5;
    		temp_T[0]=TL[i];
            for (j=0;j<7;j++)
                temp_Y[0][j]=z[i][j];
    		enthalpy_7(dummy, temp_HV, temp_Y, temp_Y, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
        	H_N[i] = temp_HV[0];
        	/*use energy balance to calculate TV*/
    		sum1=0.;
    		AAA=0.;
    		BBB=0.;
    		for (k=0;k<7;k++)
    		{
    			sum1+=HVAP[k]*y[i][k];
    			BBB+=HCAPVa[k]*MW[k]*y[i][k];
    			AAA+=0.5*HCAPVb[k]*MW[k]*y[i][k];
    		};
    		CCC=-(F_AbsGasIn*HV_AbsGasIn[0]-sumN[i]*H_N[i]-Q[i])/FV[i]+sum1;
        	if ((BBB*BBB-4*AAA*CCC)<0)
            		printf("error on vapor temp");
        	TV[i]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA));
    		temp_T[0]=TV[i];
            for (j=0;j<7;j++)
                temp_Y[0][j]=y[i][j];
            enthalpy_7(dummy, temp_HV, temp_Y, temp_Y, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    		HV[i] = temp_HV[0];
    	}/*end of i==0*/
    	else
    	{
        	//other trays
    	    sumN[i]=0.;
    		for (j=0;j<7;j++)
            {
			    if (y[i-1][j]==y_interface[i][j])
               		sign[j] = 0.; 
               	else
        			sign[j]=(y[i-1][j]-y_interface[i][j])/fabs(y[i-1][j] - y_interface[i][j]);
        		if(fabs(Nmt[i]*(y[i-1][j]-y_interface[i][j])) > 0.5*FV[i-1]*y[i-1][j])
           			minimum[i][j]=0.5*FV[i-1]*y[i-1][j];
        		else
        			minimum[i][j]= fabs(Nmt[i]*(y[i-1][j] - y_interface[i][j]));
        		N[i][j]=sign[j]*minimum[i][j];
        		sumN[i]+=N[i][j];
        	};
        	FV[i]=FV[i-1]-sumN[i];
    		for (j=0;j<7;j++)
    		{
        		y[i][j]=(FV[i-1]*y[i-1][j]-N[i][j])/FV[i];
        		z[i][j]=N[i][j]/sumN[i];
    		};
    		Q[i]=Qmt[i]*(TV[i-1]-TL[i])*9/5;
    		temp_T[0]=TL[i]; 
            for (j=0;j<7;j++)
                temp_Y[0][j]=z[i][j];
            enthalpy_7(dummy, temp_HV, temp_Y, temp_Y, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb); 
          	H_N[i] = temp_HV[0];
            /*use energy balance to calculate TV*/
    		sum1=0.;
    		AAA=0.;
    		BBB=0.;
    		for (k=0;k<7;k++)
    		{
    			sum1+=HVAP[k]*y[i][k];
    			BBB+=HCAPVa[k]*MW[k]*y[i][k];
    			AAA+=0.5*HCAPVb[k]*MW[k]*y[i][k];
    		};
    		CCC=-(FV[i-1]*HV[i-1]-sumN[i]*H_N[i]-Q[i])/FV[i]+sum1;
           	if ((BBB*BBB-4*AAA*CCC)<0)
           		printf("error on vapor temp");
        	TV[i]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA));
    		temp_T[0]=TV[i];
    		for (j=0;j<7;j++)
        		temp_Y[0][j]=y[i][j];
    		enthalpy_7(dummy, temp_HV, temp_Y, temp_Y, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
        	HV[i] = temp_HV[0];
    	}/*end of i!=0*/
    }/*end of for loop*/

    /*------------------------------------------------------------------------*/
    //calculate derivatives
    //base
    sum1=0.;
    sum2=0.;
    for (j=0;j<7;j++)
    {
    	sum1+=x[0][j]*MW[j];
    	sum2+=x_base[0][j]*MW[j];
    };
    dMbasedt=FL[0]-F_AbsLiquidOut-F_Circulation;
    dLbasedt=dMbasedt/liquid_concentration/Working_Level_Volume; /*volume percentage*/
    for (j=0;j<7;j++)
    {
    	dxbasedt[j]=(FL[0]*x[0][j]-(F_AbsLiquidOut+F_Circulation)*x_base[0][j]-x_base[0][j]*dMbasedt)/liquid_concentration/Volume_base;
        temp_X[0][j]=x[0][j];
    };
    temp_T[0]=TL[0];
    enthalpy_7(temp_HL, dummy, temp_X, temp_X, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa,HCAPVb);
    HL[0] = temp_HL[0];
    sum1=0.;
    for (j=0;j<7;j++)
    	sum1+=x_base[0][j]*MW[j]*(HCAPLa[j]+HCAPLb[j]*T_base[0]);
    dTbasedt=(FL[0]*(HL[0]-HL_base[0])-(F_AbsLiquidOut+	F_Circulation)*(HL_base[0]-HL_base[0]))/(liquid_concentration*Volume_base)/sum1;

    //trays
    for (i=0;i<NT;i++)
    {
    	if (i==NF-1) //circulation feed tray
   	    {
       		dMdt[i]=FL[i+1]+F_Circulation+sumN[i]-FL[i];
    		for (j=0;j<7;j++)
			    dxdt[i][j]=(FL[i+1]*x[i+1][j]+F_Circulation*x_Circulation[0][j] +N[i][j]-FL[i]*x[i][j]-x[i][j]*dMdt[i])/M[i];
       		sum1=0.;
    		for (j=0;j<7;j++)
		    {
    			temp_X[0][j]=x[i+1][j];
    			sum1+=x[i][j]*MW[j]*(HCAPLa[j]+HCAPLb[j]*TL[i]);
    		};
    		temp_T[0]=TL[i+1];
    		enthalpy_7(temp_HL, dummy, temp_X, temp_X, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
        	HL[i+1] = temp_HL[0];
    		dTdt[i]=(FL[i+1]*(HL[i+1]-HL[i])+F_Circulation*(HL_Circulation[0]-HL[i])+sumN[i]*(H_N[i]-HL[i])+ Q[i] - FL[i]*(HL[i]-HL[i]))/M[i]/sum1; 
    	}  
    	else if (i==NT-1) //top tray
    	{
    		dMdt[i]=F_AbsLiquidIn+sumN[i]-FL[i];
    		sum1=0.;
    		for (j=0;j<7;j++)
    		{
    			dxdt[i][j]=(F_AbsLiquidIn*x_AbsLiquidIn[j] +N[i][j]-FL[i]*x[i][j]-x[i][j]*dMdt[i])/M[i];
    			sum1+=x[i][j]*MW[j]*(HCAPLa[j]+HCAPLb[j]*TL[i]);
    		};
    		dTdt[i]=(F_AbsLiquidIn*(HL_Scrub[0]-HL[i]) + sumN[i]*(H_N[i]-HL[i]) + Q[i]-FL[i]*(HL[i]-HL[i]))/M[i]/sum1;
       	}
    	else //other trays
    	{
    		dMdt[i]=FL[i+1]+sumN[i]-FL[i];
    		for (j=0;j<7;j++)
    		{
    			dxdt[i][j]=(FL[i+1]*x[i+1][j]+N[i][j] -	FL[i]*x[i][j]-x[i][j]*dMdt[i])/M[i];
           		temp_X[0][j]=x[i+1][j];
    		};  
    		temp_T[0]=TL[i+1];
    		enthalpy_7(temp_HL, dummy, temp_X, temp_X, temp_T, MW, HVAP,HCAPLa,HCAPLb, HCAPVa, HCAPVb);
    		HL[i+1] = temp_HL[0];
    		sum1=0.;
            for (j=0;j<7;j++)
                sum1+=x[i][j]*MW[j]*(HCAPLa[j]+HCAPLb[j]*TL[i]);
    		dTdt[i]=(FL[i+1]*(HL[i+1]-HL[i]) + sumN[i]*(H_N[i]-HL[i]) +	Q[i]-FL[i]*(HL[i]-HL[i]))/M[i]/sum1;
       	}
    }/*end of for loop*/

    /*------------------------------------------------------------------------*/
    for (i=0;i<NT;i++)
    {
    	dstatedt[i]=dxdt[i][0];
    	dstatedt[i+NT]=dxdt[i][1];
    	dstatedt[i+2*NT]=dxdt[i][2];
    	dstatedt[i+3*NT]=dxdt[i][4];
    	dstatedt[i+4*NT]=dxdt[i][5];
    	dstatedt[i+5*NT]=dxdt[i][6];
    	dstatedt[i+6*NT]=dMdt[i];
    	dstatedt[i+7*NT]=dTdt[i];
    };
    dstatedt[8*NT]=dxbasedt[0];
    dstatedt[8*NT+1]=dxbasedt[1];
    dstatedt[8*NT+2]=dxbasedt[2];
    dstatedt[8*NT+3]=dxbasedt[4];
    dstatedt[8*NT+4]=dxbasedt[5];
    dstatedt[8*NT+5]=dxbasedt[6];
    dstatedt[8*NT+6]=dLbasedt;
    dstatedt[8*NT+7]=dTbasedt;
    *Level=states[8*NT+6];
    *F_AbsGasOut=FV[NT-1];
    for (j=0;j<7; j++)
    {
    	y_AbsGasOut[j]=y[NT-1][j];
    	x_AbsLiquidOut[j]=x_base[0][j];
    };
    *T_AbsGasOut=TV[NT-1];
    *T_AbsLiquidOut=T_base[0];
#ifdef DEBUG
	debug("Absorber left.\n");
#endif  
}

/*
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  double *states;
  double Pin;
  double F_AbsLiquidOut;
  double F_AbsGasIn;
  double *y_AbsGasIn;
  double T_AbsGasIn;
  double F_AbsLiquidIn;
  double *x_AbsLiquidIn;
  double T_AbsLiquidIn;
  double F_Circulation;
  double Q_Circulation;
  double Q_Scrub;
  double NT_absorber;
  double NF_absorber;
  double Working_Level_Volume;
  double *M0_absorber;
  double *L0_absorber;
  double hydtau_absorber;
  double *Nmt;
  double *Qmt;
  double *T_Circulation;
  double *T_Scrub;
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
  
  double *dstatedt_Abs;
  double *dstatedt_other;
  double *Level;
  double *TV;
  double *TL;
  double *F_AbsGasOut;
  double *y_AbsGasOut;
  double *T_AbsGasOut;
  double *x_AbsLiquidOut;
  double *T_AbsLiquidOut;
  
  if(nrhs!=37) 
    mexErrMsgTxt("inputs not correct");
  if(nlhs!=10) 
    mexErrMsgTxt("outputs not correct");
  
  states    = mxGetPr(prhs[0]);
  Pin  = mxGetScalar(prhs[1]);
  F_AbsLiquidOut = mxGetScalar(prhs[2]);
  F_AbsGasIn  = mxGetScalar(prhs[3]);
  y_AbsGasIn  = mxGetPr(prhs[4]);
  T_AbsGasIn  = mxGetScalar(prhs[5]);
  F_AbsLiquidIn  = mxGetScalar(prhs[6]);
  x_AbsLiquidIn  = mxGetPr(prhs[7]);
  T_AbsLiquidIn  = mxGetScalar(prhs[8]);
  F_Circulation  = mxGetScalar(prhs[9]);
  Q_Circulation  = mxGetScalar(prhs[10]);
  Q_Scrub  = mxGetScalar(prhs[11]);
  NT_absorber  = mxGetScalar(prhs[12]);
  NF_absorber  = mxGetScalar(prhs[13]);
  Working_Level_Volume  = mxGetScalar(prhs[14]);
  M0_absorber  = mxGetPr(prhs[15]);
  L0_absorber  = mxGetPr(prhs[16]);
  hydtau_absorber  = mxGetScalar(prhs[17]);
  Nmt  = mxGetPr(prhs[18]);
  Qmt  = mxGetPr(prhs[19]);
  T_Circulation= mxGetPr(prhs[20]);
  T_Scrub= mxGetPr(prhs[21]);
  VIJ  = mxGetPr(prhs[22]);
  aij  = mxGetPr(prhs[23]);
  MW  = mxGetPr(prhs[24]);
  SpG  = mxGetPr(prhs[25]);
  HVAP  = mxGetPr(prhs[26]);
  HCAPLa  = mxGetPr(prhs[27]);
  HCAPLb  = mxGetPr(prhs[28]);
  HCAPVa  = mxGetPr(prhs[29]);
  HCAPVb  = mxGetPr(prhs[30]);
  A  = mxGetPr(prhs[31]);
  B  = mxGetPr(prhs[32]);
  C  = mxGetPr(prhs[33]);
  R  = mxGetScalar(prhs[34]);
  Dw  = mxGetScalar(prhs[35]);
  T_ref  = mxGetScalar(prhs[36]);

  //-------------------------------------------
  mrows = mxGetM(prhs[0]);
  ncols = mxGetN(prhs[0]);
  plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  dstatedt_Abs = mxGetPr(plhs[0]);
  //-------------------------------------------
  plhs[1] = mxCreateDoubleMatrix(7,1, mxREAL);
  dstatedt_other = mxGetPr(plhs[1]);
  //-------------------------------------------
  plhs[2] = mxCreateScalarDouble(0);
  Level = mxGetPr(plhs[2]);
  //-------------------------------------------
  mrows = mxGetM(prhs[15]);
  ncols = mxGetN(prhs[15]);
  plhs[3] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  TV = mxGetPr(plhs[3]);
  //-------------------------------------------
  mrows = mxGetM(prhs[15]);
  ncols = mxGetN(prhs[15]);
  plhs[4] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  TL = mxGetPr(plhs[4]);
  //-------------------------------------------
  plhs[5] = mxCreateScalarDouble(0);
  F_AbsGasOut = mxGetPr(plhs[5]);
  //-------------------------------------------
  mrows = mxGetM(prhs[4]);
  ncols = mxGetN(prhs[4]);
  plhs[6] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  y_AbsGasOut = mxGetPr(plhs[6]);
  //-------------------------------------------
  plhs[7] = mxCreateScalarDouble(0);
  T_AbsGasOut = mxGetPr(plhs[7]);
  //-------------------------------------------
  mrows = mxGetM(prhs[7]);
  ncols = mxGetN(prhs[7]);
  plhs[8] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  x_AbsLiquidOut = mxGetPr(plhs[8]);
  //-------------------------------------------
  plhs[9] = mxCreateScalarDouble(0);
  T_AbsLiquidOut = mxGetPr(plhs[9]);
  
  Absorber(dstatedt_Abs, dstatedt_other, Level, TV, TL, F_AbsGasOut,
        y_AbsGasOut, T_AbsGasOut, x_AbsLiquidOut, T_AbsLiquidOut,
        states,Pin,F_AbsLiquidOut,F_AbsGasIn, y_AbsGasIn, T_AbsGasIn,F_AbsLiquidIn,x_AbsLiquidIn,T_AbsLiquidIn,
        F_Circulation,Q_Circulation,Q_Scrub,NT_absorber, NF_absorber, Working_Level_Volume, M0_absorber, L0_absorber,
        hydtau_absorber, Nmt, Qmt, T_Circulation, T_Scrub, VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);
}
*/