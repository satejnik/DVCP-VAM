//this is a file in the VAc dynamic model software package
//parent-routine: bubpt_column.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This function calculates VLE activity coefficient using Wilson model for three components in the distillation column

#include <stdio.h>
#include <math.h>

void gamma_wilson_3(double wilson[], double TT, double xx[], double vvij[][3], double aaij[][3], double R, double T_ref)
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
    double Lambda[3][3];
    double sLambda[3]={0.,0.,0.};
    double const_vect[3]={0.,0.,0.};
    double the_sum[3]={0.,0.,0.};
    double z[3]={0.,0.,0.};

    for (i=0;i<3;i++)
	{
    	for (j=0;j<3;j++)
        	Lambda[i][j] = vvij[i][j]*exp(-aaij[i][j]/(R*(TT + T_ref)));
	};
    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
        {	
           	sLambda[i]+=Lambda[i][j]*xx[j];
    		const_vect[i]+= Lambda[i][j]*xx[j];
		};
	};
    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
    		the_sum[i]+=(Lambda[j][i]/const_vect[j])*xx[j];
  		z[i] = 1-log(sLambda[i])-the_sum[i];
		wilson[i] = exp(z[i]);
	};
}
