//this is a file in the VAc dynamic model software package
//parent-routine: vaporizer.c vamodel.c absorber.c beta_separator.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This function calculates VLE activity coefficient using Wilson model for seven components

#include <stdio.h>
#include <math.h>
#include "debug.h"

void gamma_wilson_7(double wilson[], double TT, double xx[], double vvij[][7], double aaij[][7], double R, double T_ref)
{

    /*input*/	
    /*TT: bubble-point temperature, degC*/	
    /*x: liquid molar fraction*/
    /*VIJ: molar volume of component i / j*/
    /*aij: Wilson parameter*/
    /*R: gas constant, which is 1.987 kcal/(kmol*K)*/
    /*T_ref: absolute temperature of 0 degC, which is 273.15 K*/

    /*output*/	
    /*wilson(i,j) used in bubble-point calculation*/

    int i, j; 
    double Lambda[7][7];
    double sLambda[7]={0.,0.,0.,0.,0.,0.,0.};
    double const_vect[7]={0.,0.,0.,0.,0.,0.,0.};
    double the_sum[7]={0.,0.,0.,0.,0.,0.,0.};
    double z[7]={0.,0.,0.,0.,0.,0.,0.};
#ifdef DEBUG
	debug("gamma_wilson_7 entered.\n");
#endif  
    for (i=0;i<7;i++)
	{
    	for (j=0;j<7;j++)
        	Lambda[i][j] = vvij[i][j]*exp(-aaij[i][j]/(R*(TT + T_ref)));
	};
    for (i=0;i<7;i++)
    {
        for (j=0;j<7;j++)
        {	
           	sLambda[i]+=Lambda[i][j]*xx[j];
    		const_vect[i]+= Lambda[i][j]*xx[j];
		};
	};
    for (i=0;i<7;i++)
    {
        for (j=0;j<7;j++)
    		the_sum[i]+=(Lambda[j][i]/const_vect[j])*xx[j];
  		z[i] = 1-log(sLambda[i])-the_sum[i];
		wilson[i] = exp(z[i]);
	};
#ifdef DEBUG
	debug("gamma_wilson_7 left.\n");
#endif  
}
