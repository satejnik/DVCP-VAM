//this is a file in the VAc dynamic model software package
//parent-routine: VAmodel.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//The absorber is to simulate a gas compressor
//Since no holdup is assumed in the compressor, the mass flowrate and composition of the inlet stream is always equal to those of the outlet stream
//Outlet pressure is calculated based on a static delta_P calculation and the inlet stream pressure is known as the separator pressure which is a state variable
//Outlet temperature is calculated based on the adiabatic assumption 
//No dynamics are involved

#include <stdio.h>
#include <math.h>
#include "debug.h"

void Compressor(double F_out[], double y_out[], double T_out[], double P_out[], double F[],
    double y[], double T[], double P[], double Compressor_coefficient[], double MW[])
{
    int i;
    double sum1;
#ifdef DEBUG
	debug("Compressor entered.\n");
#endif
    F_out[0]=F[0];
    sum1=0.;
    for (i=0;i<7;i++)
    {
        y_out[i]=y[i];
        sum1+= y[i]*MW[i];
    };
    P_out[0]=(P[0]/14.696*101.325*sum1/8.314/(273.16+T[0]))*2.20462/(3.2808*3.2808*3.2808)/144*Compressor_coefficient[0]+P[0];
    T_out[0]=(P_out[0]/P[0])*(T[0]+273.15)-273.15;
#ifdef DEBUG
	debug("Compressor left.\n");
#endif
}
