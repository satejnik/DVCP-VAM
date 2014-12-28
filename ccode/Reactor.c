//this is a file in the VAc dynamic model software package
//this is the main file for the reactor
//parent-routine: VAmodel.c
//subroutines involve: reactor.c reaction_rate.c Cp.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//if a user wants to build a DLL for the reactor only, follow the instruction below:  
//1.  Add this line to the head of this file: #include "mex.h"
//2.  Enable mexFunction(), which is the second routine included in this file (currently disabled)
//3.  Use the following line to generate a DLL file: reactor.dll 
//          mex reactor.c reaction_rate.c Cp.c
//4.  The name of the generated function is "reactor()".  This function should be called in MATLAB.

//The plug-flow reactor is modeled as a 10-section distributed model

#include <stdio.h>
#include <math.h>
#include "debug.h"
//#include "mex.h"

void Reactor(double *dstatedt, double states[], double time, double Fin, double Yin[], double Tin, double Pin, double Cin, double deltap, double Shell_T,
    double Coefficient1[], double Coefficient2[], int NR, double tube_L, int tube_number, double tube_diameter, double tube_area, double UA,
    double cata_bulk_density, double cata_heatcapacity, double cata_porosity, double cata_weight, double E_r1, double E_r2,  
    double MW[], double HVAP[], double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[], double T_ref)
{

    /* model - This is a nonlinear model of a catalytic plug flow reactor with	*/
    /*           10 sections. Two exothermic reactios and seven components are 	*/
    /*           in the system including an inert.  Gas input and gas output.	*/
    /*           Model assumptions: 						*/
    /*           1. Heat is removed by shell steam so it's not an adiabatic reactor	*/
    /*           2. The heat transferred between bed and gas is ignored, therefore T_bed=T_gas like homogeneous	*/
    /*           3. Gas drop is linearized without compensation 			*/
    /*           4. Ideal gas law is enforced 			*/
    /*           This dynamic model is based on the equations (18) to (21) in Luyben's paper	*/
    /*										*/
    //--------------------------------------------------------------------------
    //physical properties: MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, T_ref
    //--------------------------------------------------------------------------
    /*output:
        dstatedt: state derivatives
    --------------------------------------------------------------------------*/
    /*input:
        states: states
        time: current running time in minute
        Fin: reactor feed flowrate
        Yin: reactor feed composition
        Tin: reactor feed temperature
        Pin: reactor feed pressure
        Cin: reactor feed concentration
        deltap: total pressure drop
        Shell_T: shell temperature
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        Coefficient1: stoichimetric coefficients for reaction 1
        Coefficient2: stoichimetric coefficients for reaction 2
        NR: section number + 1
        tube_L: tube length
        tube_number: tube number
        tube_diameter: tube diameter
        tube_area: tube cross-section area
        UA: Inverse of the Total Thermal Resistance
        cata_bulk_density: catalyst bulk density
        cata_heatcapacity: catalyst heat capacity
        cata_porosity: catalyst porosity 
        cata_weight: catalyst weight
        E_r1: heat of reaction for reaction 1
        E_r2: heat of reaction for reaction 2
    ------------------------------------------------------------------------*/

    //local variables
    //states
    double C_O2[11], C_CO2[11], C_C2H4[11], C_VAc[11], C_H2O[11], C_HAc[11], Tg[11];
    
    //intermediate variables
    double C_C2H6[11], CC[11], y[11][7], cp[11][7], P_profile[11], Gas_density[11], FCP[11], CCP[11], v[11], Q_rct[11], r_all[11][2]; 
    double dp; //prossure drop in each section
    double dz; //section length
    double G; //specific mass flowrate
    double avg_MW; //average molecular weight   
    double PR[7]; //partial pressure
    double Heat_Cap[7]; //partial heat capacity
    
    //derivatives
    double C_O2_t[11], C_CO2_t[11], C_C2H4_t[11], C_VAc_t[11], C_H2O_t[11], C_HAc_t[11], Tg_t[11];

    //others    
    int i, j, n;
    double sum1, temp_T[1], dummy[7], r[2];
#ifdef DEBUG
	debug("Reactor entered.\n");
#endif  
    /*Calculate pressure for each section, assuming linear drop*/
    dp=deltap/(NR-1);
    P_profile[0]=Pin;
    for (i=1;i<NR;i++)   
        P_profile[i]=P_profile[i-1]-dp;
    
    /*calculate mass velocity of fluid phase, (kg/min/m^2)	*/
    sum1=0.;
    for (i=0;i<7;i++)
        sum1+=MW[i]*Yin[i];
    G=Fin*sum1/tube_area;	
    Gas_density[0]=Cin*sum1;
    
    /*------------------------------------------------------------------------------*/
    /*get states*/
    C_O2[0] = Cin*Yin[0];
    C_CO2[0] = Cin*Yin[1];
    C_C2H4[0] = Cin*Yin[2];
    C_VAc[0] = Cin*Yin[4];
    C_H2O[0] = Cin*Yin[5];
    C_HAc[0] = Cin*Yin[6];
    Tg[0]=Tin;
    for (i=0;i<NR-1;i++)
    {
    	C_O2[i+1]=states[i];
    	C_CO2[i+1]=states[NR-1+i];
    	C_C2H4[i+1]=states[2*NR-2+i];
    	C_VAc[i+1]=states[3*NR-3+i];
    	C_H2O[i+1]=states[4*NR-4+i];
    	C_HAc[i+1]=states[5*NR-5+i];
    	Tg[i+1]=states[6*NR-6+i];
    };

    /*------------------------------------------------------------------------------*/
    /*get total concentration and C_C2H6							*/
    for (i=0;i<NR;i++)
    {
        CC[i]=P_profile[i]/8.314/(Tg[i]+T_ref)/14.696*101.325;
        C_C2H6[i]=CC[i]-C_O2[i]-C_CO2[i]-C_C2H4[i]-C_VAc[i]-C_H2O[i]-C_HAc[i]; 
    };

    /*------------------------------------------------------------------------------*/
    /*calculate v*/
    dz=tube_L/(NR-1); /*delta_z*/
    for (i=0;i<NR;i++)
    {
    	y[i][0]=C_O2[i]/CC[i];
    	y[i][1]=C_CO2[i]/CC[i];
    	y[i][2]=C_C2H4[i]/CC[i];
    	y[i][3]=C_C2H6[i]/CC[i];
    	y[i][4]=C_VAc[i]/CC[i];
    	y[i][5]=C_H2O[i]/CC[i];
    	y[i][6]=C_HAc[i]/CC[i];
    	avg_MW = 0.;
    	for (j=0;j<7;j++)
    		avg_MW+=MW[j]*y[i][j];	/*average mw*/
        Gas_density[i]=CC[i]*avg_MW;
        temp_T[0]=Tg[i];
    	Cp(dummy, Heat_Cap, temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb); 
    	FCP[i]=0.;
    	CCP[i]=0.;
    	for (j=0;j<7;j++)
    	{
    		cp[i][j]=Heat_Cap[j];/*kcal/kmol/c*/  
    		FCP[i]+=cp[i][j]*G/avg_MW*y[i][j];  		/*kcal/min c m^2*/
    		CCP[i]+=cp[i][j]*CC[i]*y[i][j]; 		/*kcal/m^3 c    */
    	};
    	v[i]= G /Gas_density[i]; /*m/min, assume constant mass flow per area	*/
    };

    /*------------------------------------------------------------------------------*/
    /*calculate reaction rate*/
    for (i=0;i<NR;i++)
    {
    	for (j=0;j<7;j++)
   	    	PR[j]=y[i][j]*P_profile[i]; 
    	r[0]=0.;
    	r[1]=0.;
    	temp_T[0]=Tg[i];
    	reaction_rate(r, PR, temp_T, time);
       	r_all[i][0]=r[0];
    	r_all[i][1]=r[1];
        /*This rate has unit of "kmol/min/kg catalyst" and should be turned into "kmol/m^3/min"	*/
    	r_all[i][0]=r_all[i][0]*cata_bulk_density;
    	r_all[i][1]=r_all[i][1]*cata_bulk_density;
    };
    
    /*------------------------------------------------------------------------------*/
    /*calculate derivatives*/
    for (n=1;n<NR;n++)
    {
        /*Use component balance to get concentration change:				*/
        /*dC/dt=-delta(C*v)/deltaZ+sum(vij*rj)              				*/
        /*Use single backward 								*/	
    	C_O2_t[n-1]=(-(C_O2[n]*v[n]-C_O2[n-1]*v[n-1])/dz + Coefficient1[0]*r_all[n][0]+Coefficient2[0]*r_all[n][1])/cata_porosity;
        C_CO2_t[n-1]=(-(C_CO2[n]*v[n]-C_CO2[n-1]*v[n-1])/dz + Coefficient1[1]*r_all[n][0]+Coefficient2[1]*r_all[n][1])/cata_porosity;
    	C_C2H4_t[n-1]=(-(C_C2H4[n]*v[n]-C_C2H4[n-1]*v[n-1])/dz + Coefficient1[2]*r_all[n][0]+Coefficient2[2]*r_all[n][1])/cata_porosity;
    	C_VAc_t[n-1]=(-(C_VAc[n]*v[n]-C_VAc[n-1]*v[n-1])/dz + Coefficient1[4]*r_all[n][0]+Coefficient2[4]*r_all[n][1])/cata_porosity;
    	C_H2O_t[n-1]=(-(C_H2O[n]*v[n]-C_H2O[n-1]*v[n-1])/dz + Coefficient1[5]*r_all[n][0]+Coefficient2[5]*r_all[n][1])/cata_porosity;
    	C_HAc_t[n-1]=(-(C_HAc[n]*v[n]-C_HAc[n-1]*v[n-1])/dz + Coefficient1[6]*r_all[n][0]+Coefficient2[6]*r_all[n][1])/cata_porosity;
    	Q_rct[n]= UA*(Tg[n]-Shell_T); /*kcal/min m^3*/
    	Tg_t[n-1]=1/(cata_porosity*CCP[n] + cata_heatcapacity *cata_bulk_density)*(-FCP[n]*(Tg[n]-Tg[n-1])/dz - r_all[n][0]*E_r1-r_all[n][1]*E_r2-Q_rct[n]); 
    };

    for (i=0;i<10;i++)
    {
    	dstatedt[i]=C_O2_t[i];
    	dstatedt[i+10]=C_CO2_t[i];
    	dstatedt[i+20]=C_C2H4_t[i];
    	dstatedt[i+30]=C_VAc_t[i];
    	dstatedt[i+40]=C_H2O_t[i];
    	dstatedt[i+50]=C_HAc_t[i];
    	dstatedt[i+60]=Tg_t[i];
    };
#ifdef DEBUG
	debug("Reactor left.\n");
#endif  
}

/*
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

    double *states;
    double time;
    double Fin; 
    double *Yin;
    double Tin;
    double Pin;
    double Cin;
    double deltap;
    double Shell_T;
    double *Coefficient1;
    double *Coefficient2;
    int NR;
    double tube_L;
    int tube_number;
    double tube_diameter;
    double tube_area;
    double UA;
    double cata_bulk_density;
    double cata_heatcapacity;
    double cata_porosity;
    double cata_weight;
    double E_r1;
    double E_r2;
    double *MW;
    double *HVAP;
    double *HCAPLa;
    double *HCAPLb;
    double *HCAPVa;
    double *HCAPVb;
    double T_ref;

    int mrows;
    int ncols;
  
    double *dstatedt_Rct;
    
    if(nrhs!=30) 
        mexErrMsgTxt("inputs not correct");
    if(nlhs!=1) 
        mexErrMsgTxt("outputs not correct");
  
    states = mxGetPr(prhs[0]);
    time = mxGetScalar(prhs[1]);
    Fin  = mxGetScalar(prhs[2]);
    Yin  = mxGetPr(prhs[3]);
    Tin  = mxGetScalar(prhs[4]);
    Pin  = mxGetScalar(prhs[5]);
    Cin  = mxGetScalar(prhs[6]);
    deltap = mxGetScalar(prhs[7]);
    Shell_T = mxGetScalar(prhs[8]);
    Coefficient1 = mxGetPr(prhs[9]);
    Coefficient2 = mxGetPr(prhs[10]);
    NR  = mxGetScalar(prhs[11]);
    tube_L = mxGetScalar(prhs[12]);
    tube_number = mxGetScalar(prhs[13]);
    tube_diameter = mxGetScalar(prhs[14]);
    tube_area = mxGetScalar(prhs[15]);
    UA = mxGetScalar(prhs[16]);
    cata_bulk_density = mxGetScalar(prhs[17]);
    cata_heatcapacity = mxGetScalar(prhs[18]);
    cata_porosity = mxGetScalar(prhs[19]);
    cata_weight = mxGetScalar(prhs[20]);
    E_r1 = mxGetScalar(prhs[21]);
    E_r2 = mxGetScalar(prhs[22]);
    MW  = mxGetPr(prhs[23]);
    HVAP  = mxGetPr(prhs[24]);
    HCAPLa  = mxGetPr(prhs[25]);
    HCAPLb  = mxGetPr(prhs[26]);
    HCAPVa  = mxGetPr(prhs[27]);
    HCAPVb  = mxGetPr(prhs[28]);
    T_ref  = mxGetScalar(prhs[29]);

    //-------------------------------------------
    mrows = mxGetM(prhs[0]);
    ncols = mxGetN(prhs[0]);
    plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    dstatedt_Rct = mxGetPr(plhs[0]);

    Reactor(dstatedt_Rct, states, time, Fin, Yin, Tin, Pin, Cin, deltap, 
          Shell_T, Coefficient1, Coefficient2, NR, tube_L, tube_number, tube_diameter, tube_area, UA,
          cata_bulk_density, cata_heatcapacity, cata_porosity, cata_weight, E_r1, E_r2, 
          MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, T_ref);

}
*/