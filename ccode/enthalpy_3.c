//this is a file in the VAc dynamic model software package
//parent-routine: Column.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This function calculates enthalpy for all the elements in the unit with three components present

#include <stdio.h>
#include <math.h>

void enthalpy_3(double HL[], double HV[], double x[][3], double y[][3],
    double T[], double MW[], double HVAP[], double HCAPLa[], double HCAPLb[],
    double HCAPVa[], double HCAPVb[])
{
    /*Input Parameter
    x: liquid molar fraction
    y: vapor molar fraction
    T: temperature, degC
    MW: molecular weight
    HVAP: Latent Heat
    HCAPLa, HCAPLb, HCAPVa, HCAPVb: Heat Capacity coefficients
    
    Output Parameter	
    HL: liquid mixture enthalpy, kcal/kmol
    HV: vapor mixture enthalpy, kcal/kmol*/

    int i, j;
    double mw[3], hvap[3], hcapla[3], hcaplb[3], hcapva[3], hcapvb[3];
    double hcapl[3], hcapv[3], HLL, HVV;

    for (i=4; i<7; i++)
    {
        mw[i-4] = MW[i];
        hvap[i-4] = HVAP[i];
        hcapla[i-4] = HCAPLa[i];
        hcaplb[i-4] = HCAPLb[i];
        hcapva[i-4] = HCAPVa[i];    
        hcapvb[i-4] = HCAPVb[i];
    };
    HLL=0.;
    HVV=0.;
    for (j=0;j<3; j++)
    {	
   	    hcapl[j] = (hcapla[j]*T[0] + 0.5*hcaplb[j]*pow(T[0],2))*mw[j];
       	hcapv[j] = (hcapva[j]*T[0] + 0.5*hcapvb[j]*pow(T[0],2))*mw[j] + hvap[j];
       	HLL+= x[0][j]*hcapl[j];
       	HVV+= y[0][j]*hcapv[j];
    };
    HL[0]=HLL;
    HV[0]=HVV;
}
