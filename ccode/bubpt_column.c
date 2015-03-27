//this is a file in the VAc dynamic model software package
//parent-routine: Column.c
//subroutines involve: gamma_wilson_3.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//The file is used to complete a flash calculation assuming known x and P
//the stop criterion is 1e-10
//The algorithm is based on public resource

/*	
 *	Modified by Alexander Isakov on 2015
 *	version 1.1
 *	97-99:fix for infinite loop problem: loop breaks after 100 iterations
 *	
 *	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
 *	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	----------------------------------------------------------------------
 */

#include <stdio.h>
#include <math.h>

void bubpt_column(double TT[], double YY[], double PP, double XX[],
    double AA[], double BB[], double CC[], double VVIJ[][7], double
    AAIJ[][7], double R, double T_ref)
{

    /*Input Parameter*/
    /*TTT: initial guess of the bubble-point temperature, degC*/
    /*PP: pressure, psia*/
    /*X: liquid molar fraction*/	
    /*Vi: molar volume of component i*/ 
    /*aij: Wilson parameter*/ 
    /*R: gas constant, which is 1.987 kcal/(kmol*K)*/
    /*T_ref: absolute temperature of 0 degC, which is 273.15 K*/	
    /*A,B,C: component vapor pressure antoine coefficients*/
    
    /*Output Parameter*/
    /*T: bubble-point temperature, degC*/
    /*Y: vapor molar fraction*/
    /*Only three components 5 to 7 exist in the column unit*/   

    int i, j, count;
    double sumY, f, fslope, TTT, abs_sumY1; 
    double aa[3], bb[3], cc[3], ps[3];
    double xx[3]={0., 0., 0.};
    double yy[3]={0., 0., 0.};   
    double gg[3]={0., 0., 0.};
    double vvij[3][3], aaij[3][3];
    double bbb[3]={0., 0., 0.};

    for (i=4; i<7; i++) 
    {
        aa[i-4] = AA[i];
        bb[i-4] = BB[i];
        cc[i-4] = CC[i];
        xx[i-4] = XX[i];
        yy[i-4] = YY[i];
        for (j=4; j<7; j++) 
        {
            vvij[i-4][j-4] = VVIJ[i][j];
            aaij[i-4][j-4] = AAIJ[i][j];
        } 
    } 

    TTT=TT[0];
	count = 0;
    while(1) 
    { 
        for (i=0;i<3;i++) 
            ps[i] = exp(aa[i] + bb[i]/(TTT + cc[i])); 
        gamma_wilson_3(gg, TTT, xx, vvij, aaij, R, T_ref); 
        sumY=0.;
        fslope=0.; 
        for (i=0; i<3; i++)
	    {
	        yy[i]= ps[i]*xx[i]*gg[i]/PP;
    	    sumY+= yy[i];    
    	} 
        abs_sumY1=fabs(sumY-1);
        if (abs_sumY1>1e-10)
    	{      	
        	f =  sumY*PP -PP;
        	for (j=0;j<3;j++)
		    {
    		    bbb[j] = bb[j]*xx[j]*ps[j]*gg[j]/(pow((TTT + cc[j]),2.));
    		    fslope += - bbb[j];
    		}
        	TTT = TTT - f/fslope;
        }
        else       	
        	break;
		if (count++ > 100) {
			printf("bubpt_column.c: Breaking further calculation due to infinite loop \n");
			break;
		}
    }
    for(i=4; i<7; i++)
        YY[i] = yy[i-4];
    TT[0]=TTT;
}
