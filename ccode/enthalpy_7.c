//this is a file in the VAc dynamic model software package
//parent-routine: vaporizer.c absorber.c column.c separator.c vamodel.c mixer.c 
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This function calculates enthalpy for all the elements in the unit with seven components present

#include <stdio.h>
#include <math.h>
#include "debug.h"

void enthalpy_7(double HL[], double HV[], double x[][7], double y[][7],
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

    int j;
    double hcapl[7], hcapv[7], HLL, HVV;
#ifdef DEBUG
	debug("enthalpy_7 entered.\n");
#endif
    HLL=0.;
    HVV=0.;
    for (j=0;j<7; j++)
    {	
        hcapl[j] = (HCAPLa[j]*T[0] + 0.5*HCAPLb[j]*pow(T[0],2))*MW[j];
        hcapv[j] = (HCAPVa[j]*T[0] + 0.5*HCAPVb[j]*pow(T[0],2))*MW[j] + HVAP[j];
        HLL+= x[0][j]*hcapl[j];
        HVV+= y[0][j]*hcapv[j];
    };
    HL[0]=HLL;
    HV[0]=HVV;
#ifdef DEBUG
	debug("enthalpy_7 left.\n");
#endif
}
