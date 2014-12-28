//this is a file in the VAc dynamic model software package
//this is the main file for the CO2 removal system and the gas recycle loop
//parent-routine: VAmodel.c
//subroutines involve: ComponentSeparator.c mixer.c
//Copyright: Rong Chen and Kedar Dave, June 2002
//VERSION 1.0

//The CO2 removal system is modeled by an ideal gas component separator with a variant separation ratio

#include <stdio.h>
#include <math.h>
#include "debug.h"

void CO2_Remove(double F_CO2Purge[], double y_CO2Purge[], double T_CO2Purge[], double y_Purge[], double T_Purge[], double F_ToVap[], 
                double y_ToVap[], double T_ToVap[], double F_ToFEHE[], double y_ToFEHE[], double T_ToFEHE[], 
                double F_CO2Removal_In[], double y_CO2Removal_In[], double T_CO2Removal_In[], double F_C2H4, double y_C2H4[], 
                double T_C2H4[], double bypass, double F_Purge, double F_CO2, double CO2_F_ref, double CO2_x_ref, 
                double MW[], double HVAP[], double HCAPLa[], double HCAPLb[], double HCAPVa[], double HCAPVb[]) 
{

    /* ---------------------------------------------------------------------------------------------------------
    //output
    F_CO2Purge: CO2 purge stream flowrate
    y_CO2Purge: CO2 purge stream composition
    T_CO2Purge: CO2 purge stream temperature
    y_Purge: purge stream composition
    T_Purge: purge stream temperature
    F_ToVap: gas recycle stream flowrate (through the bypass valve)
    y_ToVap: gas recycle stream composition (through the bypass valve)
    T_ToVap: gas recycle stream temperature (through the bypass valve)
    F_ToFEHE: gas recycle stream flowrate (through the FEHE)
    y_ToFEHE: gas recycle stream composition (through the FEHE)
    T_ToFEHE: gas recycle stream temperature (through the FEHE) 
    ---------------------------------------------------------------------------------------------------------*/
    /* ---------------------------------------------------------------------------------------------------------
    //input
    F_CO2Removal_In: inlet stream flowrate (absorber gas outlet stream)
    y_CO2Removal_In: inlet stream composition (absorber gas outlet stream)
    T_CO2Removal_In: inlet stream temperature (absorber gas outlet stream)
    F_C2H4: C2H4 feed flowrate
    y_C2H4: C2H4 feed composition
    T_C2H4: C2H4 feed temperature
    bypass: bypass ratio (bypass / FEHEColdInlet)
    F_Purge: purge stream flowrate
    F_CO2: CO2 component separator inlet flowrate
    ---------------------------------------------------------------------------------------------------------*/
    /* ---------------------------------------------------------------------------------------------------------
    //property
    CO2_F_ref: for CO2 component separator
    CO2_x_ref: for CO2 component separator
    ---------------------------------------------------------------------------------------------------------*/
                
    //local variable
    int i;
    double temp[1];
    double y_CO2[7], T_CO2[1]; //CO2 component separator inlet stream
    double Eff[1];  //CO2 separation ratio
    double F_CO2Exit[1], y_CO2Exit[7], T_CO2Exit[1];  //CO2 component separator outlet stream
    double F_GasRecycle[1], y_GasRecycle[7], T_GasRecycle[1];  //gas recycle stream from the absorber, before mixed with C2H4
    double F_Mixer[1], y_Mixer[7], T_Mixer[1]; //total gas recycle stream from the absorber
#ifdef DEBUG
	debug("CO2_Remove entered.\n");
#endif  
    for (i=0;i<7;i++)
        y_CO2[i]=y_CO2Removal_In[i];
    T_CO2[0]=T_CO2Removal_In[0];
    ComponentSeparator(Eff, F_CO2, y_CO2[1], CO2_F_ref, CO2_x_ref);
    F_CO2Purge[0]=F_CO2*y_CO2[1]*Eff[0];
    for (i=0;i<7;i++)
        y_CO2Purge[i]=0.;
    y_CO2Purge[1]=1.;
    T_CO2Purge[0]=T_CO2[0];
    F_CO2Exit[0]=F_CO2-F_CO2Purge[0];
    for (i=0;i<7;i++)
        y_CO2Exit[i]=(F_CO2*y_CO2[i]-F_CO2Purge[0]*y_CO2Purge[i])/F_CO2Exit[0];
    T_CO2Exit[0]=T_CO2[0];
    for (i=0;i<7;i++)
        y_Purge[i]=y_CO2Exit[i];
    T_Purge[0]=T_CO2Exit[0]; 		
    F_GasRecycle[0]=F_CO2Removal_In[0]-F_CO2Purge[0]-F_Purge;
    for (i=0;i<7;i++)
        y_GasRecycle[i]=(F_CO2Removal_In[0]*y_CO2Removal_In[i]-F_CO2Purge[0]*y_CO2Purge[i] - F_Purge*y_Purge[i])/F_GasRecycle[0];
    T_GasRecycle[0]=T_CO2Removal_In[0];
    temp[0]=F_C2H4;
    mixer(F_Mixer, y_Mixer, T_Mixer, 1, F_GasRecycle, y_GasRecycle, T_GasRecycle, temp, y_C2H4,T_C2H4, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    F_ToVap[0]=F_Mixer[0]*bypass;
    for (i=0;i<7;i++)
        y_ToVap[i]=y_Mixer[i];
    T_ToVap[0]=T_Mixer[0];
    F_ToFEHE[0]=F_Mixer[0]-F_ToVap[0];
    for (i=0;i<7;i++)
        y_ToFEHE[i]=y_Mixer[i];
    T_ToFEHE[0]=T_Mixer[0];
#ifdef DEBUG
	debug("CO2_Remove left.\n");
#endif  
}
