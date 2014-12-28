//this is a file in the VAc dynamic model software package
//parent-routine: VAmodel.c, Column.c, CO2_Remove.c
//subroutine: enthalpy_7.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is to simulate a flow junction point for two streams
//No dynamics are involved
//There is a flag "GasOrLiquid" used to indicate the phase of the flows: 1 Gas, 0 Liquid

#include <stdio.h>
#include <math.h>
#include "debug.h"

void mixer(double F_outlet[], double x_outlet[][7], double T_outlet[], int GasOrLiquid, 
    double F1[], double x1[][7], double T1[], double F2[], double x2[][7], double T2[], 
    double MW[], double HVAP[], double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[])
{
    int i;
    double AAA=0., BBB=0., CCC=0., Dummy[1]={0.}, Sum1={0.}, H1[1]={0.}, H2[1]={0.};
#ifdef DEBUG
	debug("mixer entered.\n");
#endif
    F_outlet[0]=F1[0]+F2[0];
    for (i=0;i<7;i++)
        x_outlet[0][i]=(F1[0]*x1[0][i]+F2[0]*x2[0][i])/F_outlet[0];
    if (GasOrLiquid==1) 
	{
    	/*Gas*/
    	enthalpy_7(Dummy, H1, x1, x1, T1, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);    
    	enthalpy_7(Dummy, H2, x2, x2, T2, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    	for (i=0;i<7;i++)
        {
     		Sum1+=HVAP[i]*x_outlet[0][i];
    		BBB+=HCAPVa[i]*MW[i]*x_outlet[0][i];
    		AAA+=0.5*HCAPVb[i]*MW[i]*x_outlet[0][i];
		};
    	CCC=-(F1[0]*H1[0]+F2[0]*H2[0])/F_outlet[0]+Sum1;
    	if ((BBB*BBB-4*AAA*CCC)<0)
            printf("mixer error.");
    	T_outlet[0]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/2/AAA);
   	}
	else 
	{
    	/*Liquid*/
        enthalpy_7(H1, Dummy, x1, x1, T1, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    	enthalpy_7(H2, Dummy, x2, x2, T2, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);
      	CCC=-(F1[0]*H1[0]+F2[0]*H2[0])/F_outlet[0];
    	for (i=0;i<7;i++)
		{	
	    	BBB+=HCAPLa[i]*MW[i]*x_outlet[0][i];
    		AAA+=0.5*HCAPLb[i]*MW[i]*x_outlet[0][i];
    	};
    	if ((BBB*BBB-4*AAA*CCC)<0)
        	printf("mixer error");
    	T_outlet[0]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/2/AAA);
	}
#ifdef DEBUG
	debug("mixer left.\n");
#endif
}
