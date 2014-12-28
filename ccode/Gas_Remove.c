//this is a file in the VAc dynamic model software package
//parent-routine: VAmodel.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is to simulate an ideal gas and liquid separator which doesn't exist in reality
//The four light components are splited from the three heavy components in the inlet stream and two outlet streams are formed (gas and liquid) 
//No dynamics are involved in the model

#include <stdio.h>
#include <math.h>
#include "debug.h"

void Gas_Remove(double F_GasRemoval_Out1[], double x_GasRemoval_Out1[], double T_GasRemoval_Out1[], 
        double F_GasRemoval_Out2[], double x_GasRemoval_Out2[], double T_GasRemoval_Out2[], double F[], double x[], double T[])
{

    /*output:
        F_GasRemoval_Out1: gas stream outlet flowrate
        x_GasRemoval_Out1: gas stream outlet composition
        T_GasRemoval_Out1: gas stream outlet temperature
        F_GasRemoval_Out2: liquid stream outlet flowrate
        x_GasRemoval_Out2: liquid stream outlet composition
        T_GasRemoval_Out2: liquid stream outlet temperature
    --------------------------------------------------------------------------*/
    /*input:
        F: inlet flowrate
        x: inlet composition
        T: inlet temperature
    ------------------------------------------------------------------------*/

    int i;
    double sum1, sum2;
#ifdef DEBUG
	debug("Gas_Remove entered.\n");
#endif
    sum1=0.;
    sum2=0.;
    for (i=0;i<3;i++)
    {
        sum1+= F[0]*x[i];
        sum2+= F[0]*x[i+4];
    }

    //gas
    F_GasRemoval_Out1[0]=sum1+F[0]*x[3];
    for (i=0;i<4;i++)
        x_GasRemoval_Out1[i]=F[0]*x[i]/F_GasRemoval_Out1[0];
    for (i=4;i<7;i++)
        x_GasRemoval_Out1[i]=0.;
    T_GasRemoval_Out1[0]=T[0];

    //liquid
    F_GasRemoval_Out2[0]=sum2;
    for (i=0;i<4;i++)
        x_GasRemoval_Out2[i]=0.;
    for (i=4;i<7;i++)
        x_GasRemoval_Out2[i]=F[0]*x[i]/F_GasRemoval_Out2[0];
    T_GasRemoval_Out2[0]=T[0];
#ifdef DEBUG
	debug("Gas_Remove left.\n");
#endif
}
