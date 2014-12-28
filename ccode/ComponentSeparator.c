//this is a file in the VAc dynamic model software package
//parent-routine: CO2_Remove.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is used to simulate an ideal component separator
//It calculates the separation ratio

#include <stdio.h>
#include <math.h>

void ComponentSeparator(double *Eff, double F, double x, double CO2_F_ref, double CO2_x_ref)
{
  	Eff[0]=0.995-3.14e-3*(F-CO2_F_ref)-32.5*(x-CO2_x_ref);
   	if (Eff[0]>1)
   		Eff[0]=1.;
}
