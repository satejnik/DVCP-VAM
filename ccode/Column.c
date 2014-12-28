//this is a file in the VAc dynamic model software package
//this is the main file for the azeotropic distillation column with a decanter and an HAc buffer tank
//parent-routine: VAmodel.c
//subroutines involve: bubpt_column.c enthalpy_3.c enthalpy_7.c Ludcmp.c mixer.c gamma_wilson_3.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//if a user wants to build a DLL for the column simulation only, follow the instruction below:  
//1.  Add this line to the head of this file: #include "mex.h"
//2.  Enable mexFunction(), which is the second routine included in this file (currently disabled)
//3.  Use the following line to generate a DLL: column.dll 
//          mex column.c bubpt_column.c enthalpy_3.c enthalpy_7.c Ludcmp.c mixer.c gamma_wilson_3.c
//4.  The name of the generated function is "column()".  This function should be called in MATLAB.

//The azeotropic distillation column is a lumped model assuming VLE and the vapor phase dynamics are ignored
//In the column, the tray temperature profile and the vapor flowrate profile are calculated from the state variables
//The decanter is modeled using static partition coefficient for the two liquid phases while in the column there is only one liquid phase
//The tank is a basic liquid buffer tank, modeled by using mass and energy balances

#include <stdio.h>
#include <math.h>
//#include "mex.h"

void Column(double *dstatedt, double *dstatedt_other, double *T_Column,  
    double *x_Organic, double *T_Organic, double *x_Aqueous, double *T_Aqueous, double *beta_out,
    double states[], double P[], double FL, double xF[], double TFL, 
    double LR, double QR, double LO, double LA, double LB, double T_guess[],
    double F_HAc, double x_HAc[], double T_HAc, 
    double F_Scrub, double F_VapLiquidIn, int NT, int NF, double PD, 
    double PB, double T_decanter[], double K[], double hydtau, double M0[], double LZERO[], 
    double Bottom_Working_Level_Volume, double Organic_Working_Level_Volume, 
    double Aqueous_Working_Level_Volume, double HAcTank_Working_Level_Volume, 
    double VIJ[][7], double aij[][7], double MW[], double SpG[], double HVAP[], 
    double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[], 
    double A[], double B[], double C[], double R, double Dw, double T_ref, double Q_Condenser) 
{

    //--------------------------------------------------------------------------
    //physical properties: VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref
    //--------------------------------------------------------------------------
    /*output:
        dstatedt: state derivatives
        T_Column: column temperature profile
        T_decanter: decanter temperature 
        LO_out: organic product flowrate
        x_Organic: organic product composition
        T_Organic: organic product temperature
        LA_out: aqueous product flowrate
        x_Aqueous: aqueous product composition
        T_Aqueous: aqueous product temperature
        LB_out: bottom product flowrate
        F_HAc_out: HAc fresh feed flowrate
        beta_out: decanter partition ratio
        Q_Condenser_out: condenser heat duty (duplicated when no perfect control is selected)
    --------------------------------------------------------------------------*/
    /*input:
        states: states
        P: column pressure profile
        FL: feed flowrate
        xF: feed composition
        TFL: feed temperature
        LR: reflux
        QR: reboiler duty
        LO: organic product flowrate
        LA: aqueous product flowrate
        LB: bottom flowrate
        T_guess: initial guess of the column temperature profile 
        F_HAc: HAc feed flowrate
        x_HAc[]: HAc feed composition
        T_HAc: HAc feed temperature 
        F_ToScrub: scrub flowrate
        F_VapLiquidIn: Vaporizer Liquid Inlet flowrate
        Q_Condenser: condenser heat duty
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        NT: tray number
        NF: feed tray number
        PD: top tray pressure
        PB: reboiler pressure
        K[]: decanter partition coefficients
        hydtau: hydraulic constant
        M0[]: tray holdup constant
        LZERO[]: liquid flowrate constant
        Bottom_Working_Level_Volume:
        Organic_Working_Level_Volume:
        Aqueous_Working_Level_Volume:
        HAcTank_Working_Level_Volume:
    ------------------------------------------------------------------------*/
    
    //local variables
    //states
    double x[20][3], M[20];    
    double x_bottom[1][3], Volume_bottom, x_organic[1][3], Volume_organic, x_aqueous[1][3], Volume_aqueous;
    double x_HAcTank[1][7]= {{0., 0., 0., 0., 0., 0., 0.},}, Volume_HAcTank, T_HAcTank[1];
    //intermediate variables
    double L[20], HL[20], y[20][3], V[20], HV[20], T[20], liquid_concentration, liquid_density;
    double TB[1], HLB[1], yB[1][3], VB, HVB[1], bottom_concentration, bottom_density;
    double HLO[1], organic_concentration, organic_density;
    double HLA[1], aqueous_concentration, aqueous_density;
    double HL_HAcTank[1], HL_CondenserOut[1], HV_CondenserIn[1];  
    double F_organic, F_aqueous, organic_x[3], aqueous_x[3];
    double F_HAcTankIn[1], x_HAcTankIn[1][7]={{0., 0., 0., 0., 0., 0., 0.},}, T_HAcTankIn[1], HL_HAcTankIn[1]; 
    //Feed
    double x_F[1][3], HLF[1];
    double HL_HAc[1];
    //derivatives
    double dMdt[20], dMxdt[20][3], dxdt[20][3];
    double dMdt_bottom, L_dot_bottom, dxdt_bottom[3];
    double dMdt_organic, L_dot_organic, dxdt_organic[3];
    double dMdt_aqueous, L_dot_aqueous, dxdt_aqueous[3];
    double dTdt_HAcTank, dLdt_HAcTank, dxdt_HAcTank[7], dMdt_HAcTank;  
    //others    
    int i, j, n;
    double sum1, sum2, sum3, dummy[1], AAA, BBB, CCC; 
    double temp_P, temp_HL[1], temp_HV[1], temp_T[1], temp_x[1][3], temp_y[1][3];
    double X[7]={0., 0., 0., 0., 0., 0., 0.}, Y[7]={0., 0., 0., 0., 0., 0., 0.}, X_bottom[1][7]= {{0., 0., 0., 0., 0., 0., 0.},}, X_HAc[1][7]={0., 0., 0., 0., 0., 0., 0.};
    double aaa[36]= {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,};
    double bbb[6] = {0., 0., 0., 0., 0., 0.};
    double error=1.0;
    double beta=1.0;
    int indx[6] = {0, 0, 0, 0, 0, 0};

    //Initialization
    //In the column and the decanter, three component variables are used in the calculation, while in the HAc tank, seven component variables are used
    //Tray number is counted from bottom (1) to top (20)
    for (i=0;i<NT; i++)
    {
        x[i][0]=states[i];      /*Liquid composition from btm to top*/
        x[i][2]=states[i+NT];
        x[i][1]=1-x[i][0]-x[i][2];
        M[i]=states[i+2*NT];    /*Liquid hold up from btm to top*/
    }

    x_bottom[0][0]=states[3*NT];
    x_bottom[0][2]=states[3*NT+1];
    x_bottom[0][1]=1-x_bottom[0][0]-x_bottom[0][2];
    sum1=0.;
    sum2=0.; 
    for (i=0;i<3; i++)
    {
        sum1+=x_bottom[0][i]*MW[i+4]/SpG[i+4];
        sum2+=x_bottom[0][i]*MW[i+4];
    };
    bottom_concentration=Dw/sum1;
    bottom_density=sum2*bottom_concentration;
    Volume_bottom=states[3*NT+2]*Bottom_Working_Level_Volume; /*m3*/
    
    x_organic[0][0]=states[3*NT+3];
    x_organic[0][2]=states[3*NT+4];
    x_organic[0][1]=1-x_organic[0][0]-x_organic[0][2];
    sum1=0.;
    sum2=0.;
    for (i=0;i<3; i++)
    {
        sum1+=x_organic[0][i]*MW[i+4]/SpG[i+4];
        sum2+=x_organic[0][i]*MW[i+4];
    };
    organic_concentration=Dw/sum1;
    organic_density=sum2*organic_concentration;
    Volume_organic=states[3*NT+5]*Organic_Working_Level_Volume; /*m3*/
    
    x_aqueous[0][0]=states[3*NT+6];
    x_aqueous[0][2]=states[3*NT+7];
    x_aqueous[0][1]=1-x_aqueous[0][0]-x_aqueous[0][2];
    sum1=0.;
    sum2=0.;
    for (i=0;i<3; i++)
    {
        sum1+=x_aqueous[0][i]*MW[i+4]/SpG[i+4];
        sum2+=x_aqueous[0][i]*MW[i+4];
    };
    aqueous_concentration=Dw/sum1;
    aqueous_density=sum2*aqueous_concentration;
    Volume_aqueous=states[3*NT+8]*Aqueous_Working_Level_Volume; /*m3*/
    
    x_HAcTank[0][4]=states[3*NT+9];
    x_HAcTank[0][6]=states[3*NT+10];
    x_HAcTank[0][5]=1-x_HAcTank[0][4]-x_HAcTank[0][6];
    Volume_HAcTank=states[3*NT+11]*HAcTank_Working_Level_Volume;   /*m3*/
    T_HAcTank[0]=states[3*NT+12];
    sum1=0.;
    sum2=0.;
    for (i=0;i<7; i++)
    {
        sum1+=x_HAcTank[0][i]*MW[i]/SpG[i];
        sum2+=x_HAcTank[0][i]*MW[i];
    };
    liquid_concentration=Dw/sum1;
    liquid_density=sum2*liquid_concentration;

    //use only three components in the column calculation
    for (i=0;i<3;i++)
        x_F[0][i]=xF[i+4];

    //Calculate enthalpy of the column feed, organic, aqueous, and HAc feed streams 
    enthalpy_3(temp_HL, dummy, x_F, x_F, &TFL, MW, HVAP, HCAPLa, HCAPLb,HCAPVa, HCAPVb);
    HLF[0]= temp_HL[0];
    enthalpy_3(temp_HL, dummy, x_organic, x_organic, T_decanter, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
	HLO[0]= temp_HL[0];
    enthalpy_3(temp_HL, dummy, x_aqueous, x_aqueous, T_decanter, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
	HLA[0]= temp_HL[0];
    //use seven components in the column calculation
    for (j=0;j<7;j++)
        X_HAc[0][j] = x_HAc[j];
    enthalpy_7(HL_HAc, dummy, X_HAc, X_HAc, &T_HAc, MW, HVAP, HCAPLa,HCAPLb, HCAPVa, HCAPVb);

    /* VLE calculation */
    // reboiler
    TB[0]=130.; //initial guess of the reboiler temperature
    for (j=0;j<3;j++)
        X[j+4]=x_bottom[0][j];
    bubpt_column(TB, Y, PB, X, A, B, C, VIJ, aij, R, T_ref);
    for (j=0;j<3;j++)
        yB[0][j] = Y[j+4];
    enthalpy_3(temp_HL, temp_HV, x_bottom, yB, TB, MW, HVAP, HCAPLa,HCAPLb, HCAPVa, HCAPVb);
    HLB[0]= temp_HL[0];
    HVB[0]= temp_HV[0];
    
    // column trays
    /* Get y, HL, HV and update T for each tray*/
    for (i=0;i<NT;i++)
    {
        temp_T[0]=T_guess[i];
        temp_P=P[i];
    	for (j=0;j<3;j++)
    	{
        	X[j+4]=x[i][j];
        	Y[j+4]=0.;
    	};
        bubpt_column(temp_T, Y, temp_P, X, A, B, C, VIJ, aij, R, T_ref);
    	for (j=0;j<3;j++)
    	{
        	y[i][j] = Y[j+4];
        	temp_x[0][j]=x[i][j];
        	temp_y[0][j]=y[i][j];
      	};
    	T[i]=temp_T[0];	
    	enthalpy_3(temp_HL, temp_HV, temp_x, temp_y, temp_T, MW, HVAP,HCAPLa,HCAPLb, HCAPVa, HCAPVb);
    	HL[i]= temp_HL[0];
    	HV[i]= temp_HV[0];
    };
   
    //tank
    enthalpy_7(temp_HL, dummy, x_HAcTank, x_HAcTank, T_HAcTank, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
	HL_HAcTank[0]= temp_HL[0];

	/* Liquid flows assuming linearized tray hydraulics with time constant hydtau */
    for (i=0;i<NT;i++)
        L[i] = LZERO[i] + (M[i]-M0[i])/hydtau;
    /* Vapor Flows assuming constant molar flows */
    VB=(QR+L[0]*HL[0]-LB*HLB[0])/HVB[0];
    V[0] = (HL[1]*L[1] + HVB[0]*VB - HL[0]*L[0])/HV[0];
    for (n=1;n<(NF-1);n++)
        V[n] = (HL[n+1] * L[n+1] + HV[n-1] *V[n-1] - HL[n]*L[n])/HV[n];
    V[NF-1] =(HL[NF]*L[NF] +HV[NF-2]*V[NF-2] + HLF[0]*FL - HL[NF-1]*L[NF-1])/HV[NF-1]; 
    for (n=NF;n<(NT-1);n++)
        V[n] = (HL[n +1] *L[n+1] + HV[n-1]*V[n-1] -HL[n] *L[n])/HV[n];
    V[NT-1] = (HLO[0]*LR + HV[NT-2]*V[NT-2] - HL[NT-1]*L[NT-1])/HV[NT-1];

   	temp_T[0]=T[NT-1];
  	enthalpy_3(dummy, HV_CondenserIn, y[NT-1], y[NT-1], temp_T, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
  	CCC=-(V[NT-1]*HV_CondenserIn[0]-Q_Condenser)/V[NT-1];
   	AAA=0.;
   	BBB=0.;
   	for (i=0;i<3;i++)
    {
        BBB+=HCAPLa[i+4]*MW[i+4]*y[NT-1][i];
    	AAA+=0.5*HCAPLb[i+4]*MW[i+4]*y[NT-1][i];
    };
    if ((BBB*BBB-4*AAA*CCC)<0)
    	printf("Column: error on decanter temp");
    //calculate the temperature derivative, tau=2 minute
    dstatedt_other[4]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA)-T_decanter[0])/2;

    /* decanter calculation*/
    while (fabs(error)>1e-10) 
    {
     	error=0.;
       	for (i=0;i<3;i++)	 
       	error+=(K[i]-1)*y[NT-1][i]/(1+beta*(K[i]-1));
       	beta+=0.1*error;
    };
    aaa[0]=1.;
    aaa[3]=-K[0];
    aaa[7]=1.;  
    aaa[10]=-K[1];
    aaa[14]=1.;
    aaa[17]=-K[2];
    aaa[18]=beta*V[NT-1];
    aaa[21]=(1-beta)*V[NT-1];
    aaa[25]= beta*V[NT-1];
    aaa[28]= (1-beta)*V[NT-1];
    aaa[32]= beta*V[NT-1];
    aaa[35]= (1-beta)*V[NT-1];
    bbb[3]=V[NT-1]*y[NT-1][0];
    bbb[4]=V[NT-1]*y[NT-1][1]; 
    bbb[5]=V[NT-1]*y[NT-1][2]; 
    Ludcmp(6, aaa, indx);
    Lubksb(6, aaa, indx, bbb);
    F_organic=beta*V[NT-1];
    F_aqueous=V[NT-1]-F_organic;
    for (i=0;i<3;i++)
    {
    	organic_x[i]=bbb[i];
    	aqueous_x[i]=bbb[i+3];
    };
    *beta_out=beta;
    
    //Time derivatives from  material balances for total holdup and component holdup
    /*Inside Column*/
    for (i=1;i<(NT-1);i++)
    {
        dMdt[i] = L[i+1]- L[i]+ V[i-1]- V[i];
        dMxdt[i][0]= L[i+1]*x[i+1][0] - L[i]*x[i][0] + V[i-1]*y[i-1][0] - V[i]*y[i][0];
        dMxdt[i][2]= L[i+1]*x[i+1][2] - L[i]*x[i][2] + V[i-1]*y[i-1][2] - V[i]*y[i][2];
    };
    /* Correction for feed at the feed stage */
    dMdt[NF-1] = dMdt[NF-1]  + FL;
    dMxdt[NF-1][0]= dMxdt[NF-1][0] + FL*x_F[0][0];
    dMxdt[NF-1][2]= dMxdt[NF-1][2] + FL*x_F[0][2];
    /* Bottom*/
    dMdt[0] = L[1]- L[0]+ VB - V[0];
    dMxdt[0][0]= L[1]*x[1][0] - V[0]*y[0][0] + VB*yB[0][0] - L[0]*x[0][0];
    dMxdt[0][2]= L[1]*x[1][2] - V[0]*y[0][2] + VB*yB[0][2] - L[0]*x[0][2];
    /*Top*/
    dMdt[NT-1] = V[NT-2] - L[NT-1] + LR - V[NT-1];
    dMxdt[NT-1][0]= V[NT-2]*y[NT-2][0] + LR*x_organic[0][0] - L[NT-1]*x[NT-1][0] - V[NT-1]*y[NT-1][0];
    dMxdt[NT-1][2]= V[NT-2]*y[NT-2][2] + LR*x_organic[0][2] - L[NT-1]*x[NT-1][2] - V[NT-1]*y[NT-1][2];
    /* Compute the derivative for the mole fractions from d(Mx) = x dM + M dx */
    for (i=0;i<NT;i++)
    {
        dxdt[i][0] = (dMxdt[i][0] - x[i][0]*dMdt[i] )/M[i];
        dxdt[i][2] = (dMxdt[i][2] - x[i][2]*dMdt[i] )/M[i];
    }

    /*Reboiler or Column Bottom*/
    sum1=0.;
    sum2=0.;
    sum3=0.;
    for (i=0;i<3;i++)
    {
        sum1+=x[0][i]*MW[i+4];
        sum2+=x_bottom[0][i]*MW[i+4]; 
        sum3+=yB[0][i]*MW[i+4];
    };

    dMdt_bottom= L[0] - LB - VB;
    L_dot_bottom=dMdt_bottom/bottom_concentration/Bottom_Working_Level_Volume;
    for (i=0;i<3;i++)
        dxdt_bottom[i]=(L[0]*x[0][i]-LB*x_bottom[0][i]-VB*yB[0][i]-x_bottom[0][i]*dMdt_bottom)/bottom_concentration/Volume_bottom;
    
    /*Decanter */    
    sum1=0.;
    sum2=0.;
    sum3=0.;
    for (i=0;i<3;i++)
  	{
	    sum1+= organic_x[i]*MW[i+4];
    	sum2+= x_organic[0][i]*MW[i+4];
    	sum3+= x_organic[0][i]*MW[i+4];
	};

	dMdt_organic= F_organic - LR - LO;
    L_dot_organic=dMdt_organic/organic_concentration/Organic_Working_Level_Volume;
    for (i=0;i<3;i++)
        dxdt_organic[i]=(F_organic*organic_x[i]-(LR+LO)*x_organic[0][i]-x_organic[0][i]*dMdt_organic)/organic_concentration/Volume_organic;
    sum1=0.;
    sum2=0.;
    sum3=0.;
    for (i=0;i<3;i++)
    {
        sum1+= aqueous_x[i]*MW[i+4];
        sum2+= x_aqueous[0][i]*MW[i+4];
        sum3+= x_aqueous[0][i]*MW[i+4];
    };

    dMdt_aqueous= F_aqueous -  LA;
    L_dot_aqueous=dMdt_aqueous/aqueous_concentration/Aqueous_Working_Level_Volume;
    for (i=0;i<3;i++)
        dxdt_aqueous[i]=(F_aqueous*aqueous_x[i]-LA*x_aqueous[0][i]-x_aqueous[0][i]*dMdt_aqueous)/aqueous_concentration/Volume_aqueous;

    /*HAcTank: Two input streams are first mixed*/
    for (i=0;i<3;i++)
        X_bottom[0][i+4]=x_bottom[0][i];
    sum1=0.;
    sum2=0.;
    sum3=0.;
    for (i=0;i<7;i++)
  	{
        sum1+=x_HAcTank[0][i]*MW[i]; 
    	sum2+= X_bottom[0][i]*MW[i];
    	sum3+= X_HAc[0][i]*MW[i];
	};

	mixer(F_HAcTankIn, x_HAcTankIn, T_HAcTankIn, 0, &F_HAc, X_HAc, &T_HAc, &LB, X_bottom, TB, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    enthalpy_7(HL_HAcTankIn, dummy, x_HAcTankIn, x_HAcTankIn, T_HAcTankIn, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    sum1=0.;
    sum2=0.;
    for (i=0;i<7;i++)
    {
        sum1+=x_HAcTankIn[0][i]*MW[i];
        sum2+=x_HAcTank[0][i]*MW[i]; 
    };
    dMdt_HAcTank=F_HAcTankIn[0]-(F_Scrub+F_VapLiquidIn);    
    dLdt_HAcTank=dMdt_HAcTank/liquid_concentration/HAcTank_Working_Level_Volume; 
    /*volume percentage*/
    for (i=0;i<7;i++)
        dxdt_HAcTank[i]=(F_HAcTankIn[0]*x_HAcTankIn[0][i]-(F_Scrub+F_VapLiquidIn)*x_HAcTank[0][i]-x_HAcTank[0][i]*dMdt_HAcTank)/liquid_concentration/Volume_HAcTank;
    sum1=0.;
    for (i=0;i<7;i++)
        sum1+=x_HAcTank[0][i]*MW[i]*(HCAPLa[i]+HCAPLb[i]*T_HAcTank[0]);
    dTdt_HAcTank=(F_HAcTankIn[0]*(HL_HAcTankIn[0]-HL_HAcTank[0]))/(liquid_concentration*Volume_HAcTank)/sum1;
    
    /*Output*/
    for (i=0;i<NT; i++)
    {
        dstatedt[i]=dxdt[i][0];
        dstatedt[i+NT]=dxdt[i][2];
        dstatedt[i+2*NT]=dMdt[i];
    };
    dstatedt[3*NT]=dxdt_bottom[0];
    dstatedt[3*NT+1]=dxdt_bottom[2];
    dstatedt[3*NT+2]=L_dot_bottom;
    dstatedt[3*NT+3]=dxdt_organic[0];
    dstatedt[3*NT+4]=dxdt_organic[2];
    dstatedt[3*NT+5]=L_dot_organic;
    dstatedt[3*NT+6]=dxdt_aqueous[0];
    dstatedt[3*NT+7]=dxdt_aqueous[2];
    dstatedt[3*NT+8]=L_dot_aqueous;
    dstatedt[3*NT+9]=dxdt_HAcTank[4];
    dstatedt[3*NT+10]=dxdt_HAcTank[6];
    dstatedt[3*NT+11]=dLdt_HAcTank;
    dstatedt[3*NT+12]=dTdt_HAcTank;
    for (i=0; i<NT; i++)
        T_Column[i]=T[i];
    *T_Organic=T_decanter[0];
    *T_Aqueous=T_decanter[0];
    for (i=0; i<3; i++)
    {
        x_Organic[i+4] = x_organic[0][i];
        x_Aqueous[i+4] = x_aqueous[0][i];
    }
}

/*
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  double *states;
  double *P_Column;
  double FL;
  double *xF;
  double TFL;
  double LR;
  double QR;
  double LO;
  double LA;
  double LB;
  double *T_guess;
  double F_HAc;
  double *x_HAc;
  double T_HAc;
  double F_Scrub;
  double F_VapLiquidIn;
  double NT_column;
  double NF_column;
  double P_top;
  double P_bottom;
  double *T_decanter;  
  double *K_decanter;
  double hydtau_column;
  double *M0_column;
  double *L0_column;
  double Bottom_Working_Level_Volume;
  double Organic_Working_Level_Volume;
  double Aqueous_Working_Level_Volume;
  double HAcTank_Working_Level_Volume;
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
  double Q_Condenser;
  
  int mrows;
  int ncols;
  
  double *dstatedt_Col;
  double *dstatedt_other;
  double *T_Column;
  double *x_Organic;
  double *T_Organic;
  double *x_Aqueous;
  double *T_Aqueous;
  double *beta_out;
  
  if(nrhs!=45) 
    mexErrMsgTxt("inputs not correct");
  if(nlhs!=8) 
    mexErrMsgTxt("outputs not correct");
  
  states    = mxGetPr(prhs[0]);
  P_Column  = mxGetPr(prhs[1]);
  FL  = mxGetScalar(prhs[2]);
  xF  = mxGetPr(prhs[3]);
  TFL  = mxGetScalar(prhs[4]);
  LR  = mxGetScalar(prhs[5]);
  QR  = mxGetScalar(prhs[6]);
  LO  = mxGetScalar(prhs[7]);
  LA  = mxGetScalar(prhs[8]);
  LB  = mxGetScalar(prhs[9]);
  T_guess  = mxGetPr(prhs[10]);
  F_HAc  = mxGetScalar(prhs[11]);
  x_HAc  = mxGetPr(prhs[12]);
  T_HAc  = mxGetScalar(prhs[13]);
  F_Scrub  = mxGetScalar(prhs[14]);
  F_VapLiquidIn  = mxGetScalar(prhs[15]);
  NT_column  = mxGetScalar(prhs[16]);
  NF_column  = mxGetScalar(prhs[17]);
  P_top  = mxGetScalar(prhs[18]);
  P_bottom  = mxGetScalar(prhs[19]);
  T_decanter = mxGetPr(prhs[20]);  
  K_decanter  = mxGetPr(prhs[21]);
  hydtau_column  = mxGetScalar(prhs[22]);
  M0_column  = mxGetPr(prhs[23]);
  L0_column  = mxGetPr(prhs[24]);
  Bottom_Working_Level_Volume  = mxGetScalar(prhs[25]);
  Organic_Working_Level_Volume  = mxGetScalar(prhs[26]);
  Aqueous_Working_Level_Volume  = mxGetScalar(prhs[27]);
  HAcTank_Working_Level_Volume  = mxGetScalar(prhs[28]);
  VIJ  = mxGetPr(prhs[29]);
  aij  = mxGetPr(prhs[30]);
  MW  = mxGetPr(prhs[31]);
  SpG  = mxGetPr(prhs[32]);
  HVAP  = mxGetPr(prhs[33]);
  HCAPLa  = mxGetPr(prhs[34]);
  HCAPLb  = mxGetPr(prhs[35]);
  HCAPVa  = mxGetPr(prhs[36]);
  HCAPVb  = mxGetPr(prhs[37]);
  A  = mxGetPr(prhs[38]);
  B  = mxGetPr(prhs[39]);
  C  = mxGetPr(prhs[40]);
  R  = mxGetScalar(prhs[41]);
  Dw  = mxGetScalar(prhs[42]);
  T_ref  = mxGetScalar(prhs[43]);
  Q_Condenser = mxGetScalar(prhs[44]);
  
  //-------------------------------------------
  mrows = mxGetM(prhs[0]);
  ncols = mxGetN(prhs[0]);
  plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  dstatedt_Col = mxGetPr(plhs[0]);
  //-------------------------------------------
  mrows = 7;
  ncols = 1;
  plhs[1] = mxCreateDoubleMatrix(mrows, ncols, mxREAL);
  dstatedt_other = mxGetPr(plhs[1]);
  //-------------------------------------------
  mrows = mxGetM(prhs[10]);
  ncols = mxGetN(prhs[10]);
  plhs[2] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  T_Column = mxGetPr(plhs[2]);
  //-------------------------------------------
  mrows = mxGetM(prhs[3]);
  ncols = mxGetN(prhs[3]);
  plhs[3] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  x_Organic = mxGetPr(plhs[3]);
  //-------------------------------------------
  plhs[4] = mxCreateScalarDouble(0);
  T_Organic = mxGetPr(plhs[4]);
  //-------------------------------------------
  mrows = mxGetM(prhs[3]);
  ncols = mxGetN(prhs[3]);
  plhs[5] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
  x_Aqueous = mxGetPr(plhs[5]);
  //-------------------------------------------
  plhs[6] = mxCreateScalarDouble(0);
  T_Aqueous = mxGetPr(plhs[6]);
  //-------------------------------------------
  plhs[7] = mxCreateScalarDouble(0);
  beta_out = mxGetPr(plhs[7]);
  //-------------------------------------------
  
  Column(dstatedt_Col, dstatedt_other, T_Column, x_Organic, T_Organic, x_Aqueous, T_Aqueous, beta_out,
            states, P_Column, FL,xF,TFL,LR,QR,LO,LA,LB,T_guess,F_HAc,x_HAc,T_HAc,F_Scrub,F_VapLiquidIn,
            NT_column, NF_column, P_top, P_bottom, T_decanter, K_decanter, hydtau_column, M0_column, L0_column,
            Bottom_Working_Level_Volume, Organic_Working_Level_Volume, Aqueous_Working_Level_Volume,
            HAcTank_Working_Level_Volume, VIJ, aij, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb,
            A, B, C, R, Dw, T_ref, Q_Condenser);

}

*/