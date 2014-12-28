//this is a file in the VAc dynamic model software package
//parent-routine: VAmodel.c
//subroutine: cp.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//This routine is to simulate a Feed-Effluent-Heat-Exchanger
//The NTU-Effectiveness method is used to calculate the steady state exchanger exit temperatures and the exact FEHE dynamics are not included.  
//A 5 minute time lag is added to the exit temperature measurements (sensors), to simulate temperature dynamics as a first order system.  

#include <stdio.h>
#include <math.h>
#include "debug.h"

void FEHE(double *dstatedt_other, double F_FEHEColdOut[], double y_FEHEColdOut[], double F_FEHEHotOut[], double y_FEHEHotOut[],
    double T_FEHEColdOut[], double T_FEHEHotOut[], double F_cold_in[], double x_cold_in[], double T_cold_in[], double F_hot_in[], double x_hot_in[], double T_hot_in[], 
    double UA0[], double Ref_hotflow[], double Ref_coldflow[], double Ref[], 
    double MW[], double HVAP[], double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[])
{

    //--------------------------------------------------------------------------
    //physical properties: MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb
    //--------------------------------------------------------------------------
    /*output:
        F_FEHEColdOut: FEHE cold stream outlet flowrate
        y_FEHEColdOut: FEHE cold stream outlet composition
        F_FEHEHotOut: FEHE Hot stream outlet flowrate
        y_FEHEHotOut: FEHE Hot stream outlet composition
    --------------------------------------------------------------------------*/
    /*input:
        F_cold_in: FEHE cold stream inlet flowrate
        x_cold_in: FEHE cold stream inlet composition
        T_cold_in: FEHE cold stream inlet temperature
        F_hot_in: FEHE Hot stream inlet flowrate
        x_hot_in: FEHE Hot stream inlet composition
        T_hot_in: FEHE Hot stream inlet temperature
        T_FEHEColdOut: FEHE cold stream outlet temperature
        T_FEHEHotOut: FEHE Hot stream outlet temperature
    ------------------------------------------------------------------------*/
    /*Equipment Specification
        UA0: reference of the inverse of the total thermal resistance
        Ref_hotflow: reference of the hot flowrate
        Ref_coldflow: reference of the cold flowrate
        Ref: reference of the pow()
    ------------------------------------------------------------------------*/

    //local variables
    int i;
    double avg_mw_hot, avg_mw_cold, UA1, UA2, UA, cp[7], dummy[7], Cc, Ch, NTU, e, q, T_cold_out, T_hot_out;
#ifdef DEBUG
	debug("FEHE entered.\n");
#endif  
    Cc=0.0;
    Ch=0.0;
    avg_mw_hot=0.0;
    avg_mw_cold=0.0;

    /*calculate real UA as average of UA hot and UA cold */
    for (i=0;i<7;i++)
    {
    	avg_mw_hot=avg_mw_hot+x_hot_in[i]*MW[i];
    	avg_mw_cold=avg_mw_cold+x_cold_in[i]*MW[i];
    };
    UA1=UA0[0]*pow(F_hot_in[0]*avg_mw_hot/Ref_hotflow[0], Ref[0]);
    UA2=UA0[0]*pow(F_cold_in[0]*avg_mw_cold/Ref_coldflow[0], Ref[0]);
    UA=(UA1+UA2)/2.0;

    /*Calculate T_cold_out based on the effectiveness algorithm	*/
    Cp(dummy, cp, T_cold_in, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    for (i=0;i<7;i++)
        Cc=Cc+F_cold_in[0]*x_cold_in[i]*cp[i];
    Cp(dummy, cp, T_hot_in, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    for (i=0;i<7;i++)
        Ch=Ch+F_hot_in[0]*x_hot_in[i]*cp[i];
    NTU=UA/Cc;
    e=(1-exp(-NTU*(1-Cc/Ch)))/(1-Cc/Ch*exp(-NTU*(1-Cc/Ch)));
    q=e*Cc*(T_hot_in[0]-T_cold_in[0]);
    T_cold_out=q/Cc+T_cold_in[0];
    
    /*Calculate T_hot_out based on the effectiveness algorithm	*/
    T_hot_out=T_hot_in[0]-Cc/Ch*(T_cold_out-T_cold_in[0]);

    //output
    F_FEHEHotOut[0]=F_hot_in[0];
    for (i=0;i<7;i++)
        y_FEHEHotOut[i]=x_hot_in[i];
    //calculate the temperature derivative, tau=5 minute
    dstatedt_other[6]=(T_hot_out-T_FEHEHotOut[0])/5;
    F_FEHEColdOut[0]=F_cold_in[0];
    for (i=0;i<7;i++)
        y_FEHEColdOut[i]=x_cold_in[i];
    //calculate the temperature derivative, tau=5 minute
    dstatedt_other[5]=(T_cold_out-T_FEHEColdOut[0])/5;
#ifdef DEBUG
	debug("FEHE left.\n");
#endif  
}
