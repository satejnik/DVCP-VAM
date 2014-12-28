//this is a file in the VAc dynamic model software package
//parent-routine: FEHE.c reactor.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This function calculates liquid and vapor mixture heat capacity for all the elements in the Reactor and FEHE

#include <stdio.h>
#include <math.h>

void Cp(double Cpl[], double Cpv[], double T[], double MW[], double HVAP[], 
    double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[])
{
    /*Input Parameter*/
    /*T: temperature, degC*/
    /*MW: molecular weight*/
    /*HVAP: Latent Heat*/
    /*HCAPLa, HCAPLb, HCAPVa, HCAPVb: Heat Capacity coefficients*/

    /*Output Parameter*/	
    /*Cpv: vapor mixture heat capacity, kcal/(kmol*degC)*/
    
    int i;
    double cp_v[7], cp_l[7];
    
    for (i=0;i<7;i++)
    {
        cp_l[i]=HCAPLa[i]+HCAPLb[i]*T[0];
        cp_v[i]=HCAPVa[i]+HCAPVb[i]*T[0];
        /*output vapor phase*/
        Cpl[i]=cp_l[i]*MW[i];
        Cpv[i]=cp_v[i]*MW[i];        
    };
}
