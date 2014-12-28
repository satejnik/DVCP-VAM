//this is a file in the VAc dynamic model software package
//parent-routine: reactor.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is to calculate the reaction rate

#include <stdio.h>
#include <math.h>

void reaction_rate(double r[], double PR[], double TR[], double Clock)
{
    //input:
    //TR: temperature in each section, K
    //PR: partial pressure for seven component, psia
    //Clock: current running time, minute
    
    //output:
    //r: reaction rate
    
    double yr, alpha;

    yr=Clock/60/24/365;
    if (TR[0]<180)
   	    alpha=exp(-yr/0.621);
    else   
    	alpha=exp(-(TR[0]-180)/50)*exp(-yr/0.621);
    TR[0]=TR[0]+273.15; /*Kelvin*/
    r[0] = 0.1036 * exp(-3674/TR[0]) * PR[0]*PR[2]*PR[6] * (1 + 1.7*PR[5])/((1 + 0.583*PR[0]*(1 + 1.7*PR[5])) * (1 + 6.8*PR[6]));
    r[1] = 1.9365e5 * exp(-10116/TR[0]) * PR[0] * (1 + 0.68*PR[5])/(1 +	0.76*PR[0] * (1 + 0.68*PR[5]));
    r[0]=alpha*r[0];
    r[1]=alpha*r[1];
}
