/*	this is a file in the VAc dynamic model software package
 *	this is the main file for the simulation
 *	Copyright: Rong Chen and Kedar Dave, June 2002
 *	VERSION 1.0
 *	
 *	This file integrates all the unit operation models ande completes the whole plant flowsheet
 *	Please refer to the readme file for details
 *	Please also refer to Luyben and Tyreus's previous work for other information
 */

/*	
 *	Modified by Alexander Isakov on 2014
 *	version 1.1
 *	:Disturbances can be now separately and independently enabled or disabled
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
#include <simstruc.h>
#include "debug.h"
//#include <mex.h>

void VAModel(double *dstatesdt, double *states_out, double *MVs_out, double *measurements, 
            double states[], double MVs[], double time, int is_initial, int disturbances[])
{
    //--------------------------------------------------------------------------
    /*output:
        dstatesdt: state derivatives
        states_out: states (generated in initialization)
        MVs_out: (duplicated when no perfect control is selected), (partially calculated when perfect control is selected) or (generated in initialization)
        measurements: total available measurements, which can be easily extended by a user  
    --------------------------------------------------------------------------*/

    //--------------------------------------------------------------------------
    /*input:
        states
        MVs
        time: current running time in minute
        is_initial: 1 for loading initial steady state data, 0 for dynamic simulation
        disturbances: 1-7; 0 no disturbances
    --------------------------------------------------------------------------*/
	
	int disturbance_1 = disturbances[0];
	int disturbance_2 = disturbances[1];
	int disturbance_3 = disturbances[2];
	int disturbance_4 = disturbances[3];
	int disturbance_5 = disturbances[4];
	int disturbance_6 = disturbances[5];
	int disturbance_7 = disturbances[6];
    
    /*---------------------------------constants-------------------------------*/
    //physical properties
    //Data is not the same as Table 1 in Luyben's paper
    double Vi[7] = {64.178, 37.400, 49.347, 52.866, 101.564, 18.01, 61.445};
    double AIJ[7][7]=
    {
        {0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 0., 0.},
        {0., 0., 0., 0., 0., 1384.6, -136.1},
        {0., 0., 0., 0., 2266.4, 0., 670.7},
        {0., 0., 0., 0., 726.7, 230.6, 0.},
    };
    /*Data from Table 2 (Pure component physical properties).*/
    double MW[7] = {32., 44.01, 28.052, 30.068, 86.088, 18.008, 60.052};
    double SpG[7] = {0.5, 1.18, 0.57, 0.57, 0.85, 1.0, 0.98};    /*liquid specific gravity based on the density od water at 0 degC*/
    double HVAP[7] = {2300., 2429., 1260., 1260., 8600., 10684., 5486.};
    double HCAPLa[7] = {0.3, 0.6, 0.6, 0.6, 0.44, 0.99, 0.46};
    double HCAPLb[7] = {0., 0., 0., 0., 0.0011, 0.0002, 0.0012};
    double HCAPVa[7] = {0.218, 0.23, 0.37, 0.37, 0.29, 0.56, 0.52};
    double HCAPVb[7] = {0.0001, 0., 0.0007, 0.0007, 0.0006, -0.0016, 0.0007};
    /*Data from Table 3 (Component vapor pressure antoine coefficients*/
    double A[7] = {9.2, 7.937, 9.497, 9.497, 12.6564, 14.6394, 14.5236};
    double B[7] = {0., 0., -313., -313., -2984.45, -3984.92, -4457.83};
    double C[7] = {273.16, 273.16, 273.16, 273.16, 226.66, 233.43, 258.45};
    /*Others*/
    double R = 1.987;               /*Gas constant, kcal/(kmol*K)*/
    double Dw = 999.;               /*density of water at 0 degC, kg/m3*/
    double T_ref = 273.16;          /*Reference Temperature, K*/
    
    /*---------------------------------constants-------------------------------*/
    //feed stream properties
    /*F_O2 is MV 1*/
    double y_O2[7]={1., 0., 0., 0., 0., 0., 0};
    double T_O2[1]={30.};
    double P_O2[1]={150.};
    /*F_C2H4 is MV 2*/
    double y_C2H4[7]={0., 0., 0.999, 0.001, 0., 0., 0};
    double T_C2H4[1]={30.};
    double P_C2H4[1]={128.};
    /*F_HAc is MV 3*/
    double x_HAc[7]={0., 0., 0., 0., 0., 0., 1.};
    double T_HAc[1]={30.};
    double P_HAc[1]={150.};
    
    /*---------------------------------constants-------------------------------*/
    /*Vaporizer*/
    double Total_Volume_vaporizer[1] = {17.}; /*m3, which is 600/(3.2808)^3 in TMODS, not used in our model*/
    double Working_Volume_vaporizer[1] = {4.}; /*m3, maximum measurable liquid holdup (100% level)*/
    /*-----------------------------------------------------------------------------*/
    /*Reactor*/
    double friction_factor[1]={0.000794745091749963}; /*0.00147*2.20462*(3.2808)^3/144: deltaP=friction_factor*density*volumetric flowrate*volumetric flowrate*/
    int NR[1] = {11}; 
    double tube_L[1] ={10.};
    int tube_number[1] = {622};
    double tube_diameter[1] ={0.0371};
    double tube_area[1] = {0.67240058914293}; /*tube_area=1/4*pi*tube_diameter*tube_diameter*tube_number. total intersection area, m^2*/
    double UA_reactor[1] = {269.839144893182}; /*UA=4000*0.252*9/5/10/tube_area/1;   kcal/min m^3 degC*/
    double cata_bulk_density[1] = {385.};
    double cata_heatcapacity[1] = {0.23};
    double cata_porosity[1] = {0.8};
    double cata_weight[1] = {2588.74226820028}; /*cata_weight=tube_area*tube_L*cata_bulk_density;*/
    double E_r1[1] = {-42100.}; 	/*heat of reaction for r1, kcal/kmol*/
    double E_r2[1] = {-316000.};	/*heat of reaction for r2, kcal/kmol*/
    double sto1[7]={-0.5, 0., -1., 0., 1., 1., -1.};
    double sto2[7]={-3., 2., -1., 0., 0., 2., 0.};
    /*-----------------------------------------------------------------------------*/
    /*FEHE*/
    double UA_FEHE[1]={6483./60.0};     /*kcal/min degC*/
    double Ref_hotflow[1]={589.670};    /*kg/min*/
    double Ref_coolflow[1]={498.952};   /*kg/min*/
    double Ref[1]={0.8};
    /*-----------------------------------------------------------------------------*/
    /*Separator*/
    double Total_Volume_separator[1]={15.};     /*m3, which is 535/3.2808/3.2808/3.2808 in TMODS, not used in our model*/
    double Working_Volume_separator[1]={8.};    /*m3, maximum measurable liquid holdup (100%  level)*/
    double Total_Gas_Loop_Volume[1]={170.};     /*m3, which is 6002/3.2808/3.2808/3.2808 in TMODS*/
    double UA_separator[1]={20007.*0.252*9/5};   /*kcal/min degC*/  
    /*-----------------------------------------------------------------------------*/
    /*Compressor*/
    double Compressor_coefficient[1]={15000.};   /*delta_P=Compressor_coefficient*density/144*/
    /*-----------------------------------------------------------------------------*/
    /*Absorber*/
    int NT_absorber[1]={8};
    int NF_absorber[1]={2};
    double Working_Volume_absorber[1]={8.5};    /*m3, which is 300/3.2808/3.2808/3.2808 in TMODS*/
    double M0_absorber[8]={30/2.20462, 30/2.20462, 30/2.20462, 30/2.20462, 30/2.20462, 30/2.20462, 30/2.20462, 30/2.20462};
    double L0_absorber[8]={16.327, 16.311, 0.911, 0.885, 0.853, 0.844, 0.829, 0.815};
    double hydtau_absorber[1]={0.1};
    double Nmt[8]={60/2.20462, 60/2.20462, 60/2.20462, 60/2.20462, 60/2.20462,60/2.20462, 60/2.20462, 60/2.20462};
    double Qmt[8]={400*0.252, 400*0.252, 200*0.252, 200*0.252, 200*0.252, 200*0.252, 200*0.252, 200*0.252};
    /*-----------------------------------------------------------------------------*/
    /*CO2 Remove*/
    double CO2_F_ref[1]={6.41360260517487};
    double CO2_x_ref[1]={0.0134241598608995};
    /*-----------------------------------------------------------------------------*/
    /*Column*/
    int NT_column[1]={20};
    int NF_column[1]={15};
    double P_top[1]={18.};
    double P_bottom[1]={30.};
    double K_decanter[3]={395., 0.05, 1.}; /*decanter partition coefficients*/
    double hydtau_column[1]={0.1};
    double M0_column[20]; /*Tray holdup (kmol)*/
    double L0_column[20];
    double Bottom_Working_Level_Volume[1]={5.66};/*m3, which is 200/3.2808/3.2808/3.2808 in TMODS*/
    double Organic_Working_Level_Volume[1]={1.7};/*m3, which is 60/3.2808/3.2808/3.2808 in TMODS*/
    double Aqueous_Working_Level_Volume[1]={1.7};/*m3, which is 60/3.2808/3.2808/3.2808 in TMODS*/
    double HAcTank_Working_Level_Volume[1]={2.83};/*m3, which is 100/3.2808/3.2808/3.2808 in TMODS*/
    double T_Column[20]={131, 121, 106, 96, 93, 92, 91, 90, 89, 88, 88, 87, 86, 85, 84, 80, 78, 77, 76, 75};  //initial guess of column temperature profile
    /*---------------------------------constants-------------------------------*/


    /*---------------------------------variables-------------------------------*/
    //vaporizer states
    double x_vaporizer[7], hup_vaporizer[1], T_vaporizer[1];
    /*---------------------------------variables-------------------------------*/
    //reactor states 
    double C_O2[11], C_CO2[11], C_C2H4[11], C_VAc[11], C_H2O[11], C_HAc[11], T_reactor[11];
    /*---------------------------------variables-------------------------------*/
    //separator states 
    double x_separator[7], hup_separator[1], TL_separator[1];
    double y_separator[7], P_separator[1], TV_separator[1];
    /*---------------------------------variables-------------------------------*/
    //absorber states 
    double x_absorber[8][7], M_absorber[8], TL_absorber[8];    double x_base[7], hup_base[1], T_base[1];
    /*---------------------------------variables-------------------------------*/
    //column, decanter and HAc tank states 
    double x_column[20][7], M_column[20], x_bottom[7], hup_bottom[1];
    double x_organic[7], hup_organic[1], x_aqueous[7], hup_aqueous[1];
    double x_HAcTank[7], hup_HAcTank[1], T_HAcTank[1];
    /*---------------------------------variables-------------------------------*/
    //other states 
    double T_VapHeaterOut[1], T_ABSIN_Gas[1], T_Circulation[1], T_Scrub[1], T_decanter[1], T_FEHEColdOut[1], T_SEPIN[1]; 
    /*---------------------------------variables-------------------------------*/
    
    
    /*---------------------------------MVs-------------------------------*/
    double F_O2, F_C2H4, F_HAc, Q_Duty_Vaporizer, F_vaporizer, Q_Heater;
    double Shell_T_RCT, F_SepLiquidOUT, Shell_T_Sep, F_SepVaporOUT;
    double Q_Compressor, F_AbsLiquidOut, F_Circulation, Q_Circulation, F_Scrub, Q_Scrub;
    double F_CO2, F_Purge, bypass, F_Reflux, Q_Reboiler, F_Organic, F_Aqueous, F_Bottom, Q_Condenser, F_VapLiquidIn; 
    /*---------------------------------MVs-------------------------------*/
    
    
    /*---------------------------------Other Variables-------------------------------*/
    double F_GasRemovalIn[1], x_GasRemovalIn[7], T_GasRemovalIn[1];
    double F_ABSIN_Gas2[1], y_ABSIN_Gas2[7], T_ABSIN_Gas2[1];
    double F_COLIN[1], x_COLIN[7], T_COLIN[1];
    double F_CompressorIn[1],y_CompressorIn[7],T_CompressorIn[1];
    double F_ABSIN_Gas[1], y_ABSIN_Gas[7], T_CompCoolerIn[1], P_recycle[1];
    double dummy[1], HV_ABSIN_Gas[1], temp_x_HAcTank[7]={0.,0.,0.,0.,0.,0.,0.};
    double temp_HV[1], HV_CompCoolerIn[1];
    double AAA, BBB, CCC;
    double Level[1], TV[8], TL[8];
    double F_AbsGasOut[1], y_AbsGasOut[7], T_AbsGasOut[1], x_AbsLiquidOut[7];
    double T_AbsLiquidOut[1];
    double F_CO2Purge[1], y_CO2Purge[7], T_CO2Purge[1], y_Purge[7], T_Purge[1], F_ToVap[1], y_ToVap[7];
    double T_ToVap[1], F_FEHEIN_cold[1], y_FEHEIN_cold[7], T_FEHEIN_cold[1];
    double ps[7], g[7];
    double P_vaporizer[1], y_vaporizer[7], HV_vaporizer[1], HV_heaterOut[1];
    double F_RCTIN[1], y_RCTIN[7], T_RCTIN[1], P_RCTIN, C_RCTIN, P_Drop, r_RCTIN, G_RCTIN, V_RCTIN;
    double C_FEHEIN_hot, F_FEHEIN_hot[1], y_FEHEIN_hot[7], T_FEHEIN_hot[1], F_FEHEColdOut[1], y_FEHEColdOut[7];
    double F_SEPIN[1], y_SEPIN[7], F_VapVapIn[1], y_VapVapIn[7], T_VapVapIn[1], x_VapLiquidIn[7]={0.,0.,0.,0.,0.,0.,0.}, T_VapLiquidIn[1]; 
    double P_Column[20], beta_out[1];
    double Level_Vaporizer[1], y_Vaporizer[7], T_Vaporizer[1], P_Vaporizer[1];
    double x_Organic[7], T_Organic[1], x_Aqueous[7], T_Aqueous[1];
    double states_abs[72], states_vap[8], states_rct[70], states_sep[16], states_col[73]; 
    double dstatesdt_abs[72], dstatesdt_vap[8], dstatesdt_rct[80], dstatesdt_sep[16], dstatesdt_col[73], dstatesdt_other[7];
    double Q_Circulation_out[1], Q_Scrub_out[1], Q_Condenser_out[1];
    int i, j;
    double sum1, sum2, sum3, temp_single_1[1], temp_single_2[1];

    double VIJ[7][7];
#ifdef DEBUG
	debug("VAModel entered.\n");
#endif
    for (i=0;i<7;i++)
    {
        for (j=0;j<7;j++)
            VIJ[i][j] = Vi[j]/Vi[i];
    };
    for (i=0;i<NF_column[0];i++)
    {
    	M0_column[i]=2.3;   
    	L0_column[i]=8.7947;
    };
    for (i=NF_column[0];i<NT_column[0];i++)
    {
    	M0_column[i]=2.3;
    	L0_column[i]=4.9741;
    }

    //if disturbance 1
    if ((disturbance_1) && (is_initial!=1))
    {
        y_C2H4[2]=0.997;
        y_C2H4[3]=0.003;
    }
    //if disturbance 3
    if ((disturbance_3) && (is_initial!=1))
    {
        MVs[2]=0;
    }
    //if disturbance 4
    if ((disturbance_4) && (is_initial!=1))
    {
        MVs[0]=0;
    }

    /*---------------------------------variables-------------------------------*/

    /*--------------------------------------------------------------------------------------*/
    /*load initial states and MVs if requested                                      	    */
    /*--------------------------------------------------------------------------------------*/
    if (is_initial==1)
    {
        time=0; //make sure time=0 in order to calculate process measurements
        steadystate(MVs, states); /*load steadystate*/
    };
    
    /*--------------------------------------------------------------------------------------*/
    /*set states variables 									*/
    /*--------------------------------------------------------------------------------------*/
    /*The Vaporizer has 8 states								*/
    x_vaporizer[0] = states[0];
    x_vaporizer[1] = states[1];
    x_vaporizer[2] = states[2];
    x_vaporizer[4] = states[3];
    x_vaporizer[5] = states[4];
    x_vaporizer[6] = states[5];
    hup_vaporizer[0]=states[6];                       
    T_vaporizer[0]=states[7];
    /*The Reactor has 70 states                                                            */
    for (i=0;i<NR[0]-1;i++)
    {
        C_O2[i+1]=states[i+8];
        C_CO2[i+1]=states[NR[0]-1+i+8];
        C_C2H4[i+1]=states[2*NR[0]-2+i+8];
        C_VAc[i+1]=states[3*NR[0]-3+i+8];
        C_H2O[i+1]=states[4*NR[0]-4+i+8];
        C_HAc[i+1]=states[5*NR[0]-5+i+8];
        T_reactor[i+1]=states[6*NR[0]-6+i+8];
    };
    /*The Separator has 16 states                                                            */
    for (i=0;i<3;i++)
    {
        x_separator[i]=states[i+78];
        x_separator[i+4]=states[i+3+78];
    };
    hup_separator[0]=states[84];           
    TL_separator[0]=states[85];
    for (i=0;i<3;i++)
    {
        y_separator[i]=states[i+86];
        y_separator[i+4]=states[i+3+86];
    };
    P_separator[0]=states[92];
    TV_separator[0]=states[93];
    /*The Absorber has 72 states                                                            */
    for (i=0;i<NT_absorber[0];i++)
    {
        x_absorber[i][0]=states[i+94];
        x_absorber[i][1]=states[i+NT_absorber[0]+94];
        x_absorber[i][2]=states[i+2*NT_absorber[0]+94];
        x_absorber[i][4]=states[i+3*NT_absorber[0]+94];   
        x_absorber[i][5]=states[i+4*NT_absorber[0]+94];
        x_absorber[i][6]=states[i+5*NT_absorber[0]+94];
        M_absorber[i]=states[i+6*NT_absorber[0]+94];
        TL_absorber[i]=states[i+7*NT_absorber[0]+94];
    };
    x_base[0]=states[8*NT_absorber[0]+94];
    x_base[1]=states[8*NT_absorber[0]+1+94];
    x_base[2]=states[8*NT_absorber[0]+2+94];
    x_base[4]=states[8*NT_absorber[0]+3+94];
    x_base[5]=states[8*NT_absorber[0]+4+94];
    x_base[6]=states[8*NT_absorber[0]+5+94]; 
    hup_base[0]=states[164];
    T_base[0]=states[165];
    /*The Column has 69 states     								*/
    for (i=0;i<NT_column[0]; i++)
    {
        x_column[i][0]=states[i+166];
        x_column[i][2]=states[i+NT_column[0]+166];
        M_column[i]=states[i+2*NT_column[0]+166];
    };
    x_bottom[0]=states[226];
    x_bottom[2]=states[227];
    hup_bottom[0]=states[228];
    x_organic[0]=states[229];
    x_organic[2]=states[230];
    hup_organic[0]=states[231];
    x_aqueous[0]=states[232];
    x_aqueous[2]=states[233];
    hup_aqueous[0]=states[234];            
    /*The HAc Tank has 4 states                                                              */
    x_HAcTank[0]=states[235];
    x_HAcTank[2]=states[236];
    hup_HAcTank[0]=states[237];            
    T_HAcTank[0]=states[238];
    /*states for heater/cooler temperatures               								*/
    T_VapHeaterOut[0]=states[239];
    T_ABSIN_Gas[0]=states[240];
    T_Circulation[0]=states[241];
    T_Scrub[0]=states[242];
    T_decanter[0]=states[243]; 
    T_FEHEColdOut[0]=states[244];
    T_SEPIN[0]=states[245];

    /*--------------------------------------------------------------------------------------*/
    /*set MVs                                        										*/	
    /*--------------------------------------------------------------------------------------*/
    F_O2=MVs[0];                    
    F_C2H4=MVs[1];                  
    F_HAc=MVs[2];                   
    Q_Duty_Vaporizer=MVs[3];        
    F_vaporizer=MVs[4];             
    Q_Heater=MVs[5];                
    Shell_T_RCT=MVs[6];             
    F_SepLiquidOUT=MVs[7];          
    Shell_T_Sep=MVs[8];             
    F_SepVaporOUT=MVs[9];           
    Q_Compressor=MVs[10];           
    F_AbsLiquidOut=MVs[11];         
    F_Circulation=MVs[12];          
    Q_Circulation=MVs[13];          
    F_Scrub=MVs[14];                
    Q_Scrub=MVs[15];                
    F_CO2=MVs[16];                  
    F_Purge=MVs[17];                
    bypass=MVs[18];                 
    F_Reflux=MVs[19];               
    Q_Reboiler=MVs[20];             
    Q_Condenser=MVs[21];            
    F_Organic=MVs[22];              
    F_Aqueous=MVs[23];              
    F_Bottom=MVs[24];               
    F_VapLiquidIn=MVs[25];          
    /*------------------------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------------------------*/
    //get full values of separator liquid composition and absorber base liquid composition 
    sum1=0.;
    sum2=0.;
    x_separator[3]=0.;
    x_base[3]=0.;
    for (i=0;i<7;i++)
    {
        sum1+=x_separator[i];
        sum2+=x_base[i];
    };
    x_separator[3]=1-sum1;
    x_base[3]=1-sum2;
    
    //get mixer output  
    temp_single_1[0]=F_SepLiquidOUT;
    temp_single_2[0]=F_AbsLiquidOut;    
    mixer(F_GasRemovalIn,x_GasRemovalIn,T_GasRemovalIn, 0, temp_single_1, x_separator, TL_separator,temp_single_2,x_base,T_base,MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);

    //calculate gas remove   
    Gas_Remove(F_ABSIN_Gas2, y_ABSIN_Gas2, T_ABSIN_Gas2, F_COLIN, x_COLIN, T_COLIN, F_GasRemovalIn,x_GasRemovalIn,T_GasRemovalIn);
    //if disturbance 2
    if ((disturbance_2) && (is_initial!=1))
    {
        F_COLIN[0]=0;
    }

    //get full values of separator vapor composition
    sum1=0.;
    y_separator[3]=0.;
    for (i=0;i<7;i++)
        sum1+=y_separator[i];
    y_separator[3]=1-sum1;

    //get mixer output  
    temp_single_1[0]=F_SepVaporOUT;    
    mixer(F_CompressorIn,y_CompressorIn,T_CompressorIn, 1, temp_single_1,y_separator,TV_separator,F_ABSIN_Gas2,y_ABSIN_Gas2,T_ABSIN_Gas2, MW, HVAP,HCAPLa, HCAPLb, HCAPVa, HCAPVb);

    //calculate compressor
    Compressor(F_ABSIN_Gas,y_ABSIN_Gas,T_CompCoolerIn, P_recycle, F_CompressorIn,y_CompressorIn,TV_separator, P_separator, Compressor_coefficient, MW);
    
    //calculate compressor heat duty
    enthalpy_7(dummy, temp_HV, y_ABSIN_Gas, y_ABSIN_Gas, T_CompCoolerIn, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    HV_CompCoolerIn[0] = temp_HV[0];
   	sum1=0.;
   	BBB=0.;
   	AAA=0.;
   	for (i=0;i<7;i++)
   	{
   		sum1+=HVAP[i]*y_ABSIN_Gas[i];	
   		BBB+=HCAPVa[i]*MW[i]*y_ABSIN_Gas[i];
   		AAA+=0.5*HCAPVb[i]*MW[i]*y_ABSIN_Gas[i];
   	}
   	CCC=-(F_ABSIN_Gas[0]*HV_CompCoolerIn[0]-Q_Compressor)/F_ABSIN_Gas[0]+sum1;
   	if ((BBB*BBB-4*AAA*CCC)<0)
   		printf("error on compressor temp");
  	//calculate the temperature derivative, tau=2 minute
  	dstatesdt_other[1]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA)-T_ABSIN_Gas[0])/2;

    //get full values of HAc tank liquid composition from 3 to 7
    sum1=0.;
    x_HAcTank[1]=0.;
    for (i=0;i<3;i++)
        sum1+=x_HAcTank[i];
    x_HAcTank[1]=1-sum1;
    temp_x_HAcTank[4] = x_HAcTank[0];
    temp_x_HAcTank[5] = x_HAcTank[1];
    temp_x_HAcTank[6] = x_HAcTank[2];

    //Calculate absorber
    for (i=94;i<166;i++)
    {
        states_abs[i-94] = states[i];
    }
    Absorber(dstatesdt_abs, dstatesdt_other, Level, TV, TL, F_AbsGasOut,
        y_AbsGasOut, T_AbsGasOut, x_AbsLiquidOut, T_AbsLiquidOut,
        states_abs,P_recycle[0],F_AbsLiquidOut,F_ABSIN_Gas[0], y_ABSIN_Gas, T_ABSIN_Gas[0], F_Scrub, temp_x_HAcTank, T_HAcTank[0],
        F_Circulation,Q_Circulation,Q_Scrub,NT_absorber[0], NF_absorber[0], Working_Volume_absorber[0], M0_absorber, L0_absorber,
        hydtau_absorber[0], Nmt, Qmt, T_Circulation, T_Scrub, VIJ, AIJ, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);
    
    //Calculate CO2_Remove
    CO2_Remove(F_CO2Purge, y_CO2Purge, T_CO2Purge, y_Purge, T_Purge, F_ToVap, y_ToVap, T_ToVap,
            F_FEHEIN_cold, y_FEHEIN_cold, T_FEHEIN_cold, F_AbsGasOut, y_AbsGasOut,T_AbsGasOut, F_C2H4,
            y_C2H4, T_C2H4, bypass, F_Purge, F_CO2, CO2_F_ref[0], CO2_x_ref[0], MW, HVAP, HCAPLa, HCAPLb,  HCAPVa, HCAPVb);

    //get full values of vaporizer liquid composition
    sum1=0.;
    x_vaporizer[3]=0.;
    for (i=0;i<7;i++)  
        sum1+=x_vaporizer[i];
    x_vaporizer[3]=1-sum1;

    //Calculate reactor input    
    for (i=0;i<7;i++)
        ps[i]=exp(A[i]+B[i]/(T_vaporizer[0]+C[i]));
    gamma_wilson_7(g, T_vaporizer[0], x_vaporizer, VIJ, AIJ, R, T_ref);
    sum1=0.;
    for (i=0;i<7;i++)
        sum1+=x_vaporizer[i]*ps[i]*g[i];
    P_vaporizer[0]=sum1;
    for (i=0;i<7;i++)
        y_vaporizer[i]=x_vaporizer[i]*ps[i]*g[i]/P_vaporizer[0];

    enthalpy_7(dummy, temp_HV, y_vaporizer, y_vaporizer, T_vaporizer, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    HV_vaporizer[0] = temp_HV[0];
    sum1=0.;
    BBB=0.;
    AAA=0.;
    for (i=0;i<7;i++)
    {
       	sum1+=HVAP[i]*y_vaporizer[i];	
       	BBB+=HCAPVa[i]*MW[i]*y_vaporizer[i];
        AAA+=0.5*HCAPVb[i]*MW[i]*y_vaporizer[i];
    };
    CCC=-(F_vaporizer*HV_vaporizer[0]+Q_Heater)/F_vaporizer + sum1;
    if ((BBB*BBB-4*AAA*CCC)<0)
        printf("error on vaporizer heater temp");
    //calculate the temperature derivative, tau=2 minute
    dstatesdt_other[0]=((-BBB+sqrt(BBB*BBB-4*AAA*CCC))/(2*AAA)-T_VapHeaterOut[0])/2;

    temp_single_1[0]=F_vaporizer;
    temp_single_2[0]=F_O2;
    mixer(F_RCTIN,y_RCTIN,T_RCTIN, 1, temp_single_1, y_vaporizer,T_VapHeaterOut, temp_single_2, y_O2, T_O2, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    P_RCTIN=P_vaporizer[0];
    C_RCTIN=(P_RCTIN/14.696*101.325)/(8.314*(T_RCTIN[0]+T_ref));  /*Ideal Gas Law*/ 
    sum1=0.;
    for (i=0;i<7;i++)
        sum1+=MW[i]*y_RCTIN[i];
    G_RCTIN=F_RCTIN[0]*sum1;
    V_RCTIN=F_RCTIN[0]*8.314*(T_RCTIN[0]+273.15)/(P_RCTIN/14.696*101.325); /*volume flowrate*/
    r_RCTIN=G_RCTIN/V_RCTIN; /*density*/
    P_Drop=friction_factor[0]*r_RCTIN*V_RCTIN*V_RCTIN;  

    //Calculate FEHE input    
    T_FEHEIN_hot[0]=T_reactor[NR[0]-1];
    C_FEHEIN_hot = (P_RCTIN-P_Drop)/14.696*101.325/(8.314*(T_FEHEIN_hot[0]+T_ref));  /*Ideal Gas Law*/ 
    y_FEHEIN_hot[0] = C_O2[NR[0]-1]/C_FEHEIN_hot;
    y_FEHEIN_hot[1] = C_CO2[NR[0]-1]/C_FEHEIN_hot; 
    y_FEHEIN_hot[2] = C_C2H4[NR[0]-1]/C_FEHEIN_hot;
    y_FEHEIN_hot[4] = C_VAc[NR[0]-1]/C_FEHEIN_hot;
    y_FEHEIN_hot[5] = C_H2O[NR[0]-1]/C_FEHEIN_hot; 
    y_FEHEIN_hot[6] = C_HAc[NR[0]-1]/C_FEHEIN_hot;
    y_FEHEIN_hot[3] = 1-y_FEHEIN_hot[0]-y_FEHEIN_hot[1]-y_FEHEIN_hot[2]-y_FEHEIN_hot[4]-y_FEHEIN_hot[5]-y_FEHEIN_hot[6];
    sum1=0.;
    for (i=0;i<7;i++)
        sum1+=MW[i]*y_FEHEIN_hot[i];
    F_FEHEIN_hot[0]=G_RCTIN/sum1;
    
    //Calculate FEHE
    FEHE(dstatesdt_other,F_FEHEColdOut,y_FEHEColdOut, F_SEPIN, y_SEPIN, T_FEHEColdOut, T_SEPIN, F_FEHEIN_cold, y_FEHEIN_cold, T_FEHEIN_cold, F_FEHEIN_hot, y_FEHEIN_hot, T_FEHEIN_hot, UA_FEHE, Ref_hotflow, Ref_coolflow, Ref, MW, HVAP, HCAPLa, HCAPLb, HCAPVa,HCAPVb);
        
    //Calculate Vaporizer vapor input
    mixer(F_VapVapIn,y_VapVapIn,T_VapVapIn, 1, F_FEHEColdOut,y_FEHEColdOut,T_FEHEColdOut,F_ToVap,y_ToVap, T_ToVap, MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb);
    
    //Calculate Vaporizer liquid input
    x_VapLiquidIn[4]=x_HAcTank[0];
    x_VapLiquidIn[5]=x_HAcTank[1];
    x_VapLiquidIn[6]=x_HAcTank[2];
    T_VapLiquidIn[0]=T_HAcTank[0];

    //Calculate column pressure profile
    for (i=0;i<NT_column[0];i++)
        P_Column[i]=29.4-i*0.6; 

    //get full values of column liquid composition        
    sum1=0.;
    sum2=0.;
    sum3=0.;
    x_bottom[1]=0.;
    x_organic[1]=0.;
    x_aqueous[1]=0.;
    for (i=0;i<3;i++)
    {
        sum1+=x_bottom[i];
        sum2+=x_organic[i];
        sum3+=x_aqueous[i];
    };
    x_bottom[1]=1-sum1;
    x_organic[1]=1-sum2;
    x_aqueous[1]=1-sum3;

    /*---------------------------------------------------------------------------------------------*/
    //unit operation dynamics
    for (i=0;i<8;i++)
    {
        states_vap[i] = states[i];
        dstatesdt_vap[i]=0;        
    }
    Vaporizer(dstatesdt_vap,Level_Vaporizer,y_Vaporizer,T_Vaporizer,P_Vaporizer,
            states_vap, F_vaporizer, Q_Duty_Vaporizer, F_VapVapIn[0], y_VapVapIn, T_VapVapIn[0], 
            F_VapLiquidIn, x_VapLiquidIn, T_VapLiquidIn[0], Working_Volume_vaporizer[0], 
            VIJ, AIJ, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);
    
    for (i=8;i<78;i++)
    {
        states_rct[i-8] = states[i];
    }
    Reactor(dstatesdt_rct, states_rct, time, F_RCTIN[0], y_RCTIN, T_RCTIN[0],
            P_RCTIN, C_RCTIN, P_Drop, Shell_T_RCT, sto1, sto2, NR[0], tube_L[0],
            tube_number[0], tube_diameter[0], tube_area[0], UA_reactor[0], cata_bulk_density[0],
            cata_heatcapacity[0], cata_porosity[0], cata_weight[0], E_r1[0], E_r2[0],
            MW, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, T_ref);
    
    for (i=78;i<94;i++)
    {
        states_sep[i-78] = states[i];
        dstatesdt_sep[i-78]=0;
    } 
    Separator(dstatesdt_sep, states_sep, F_SEPIN[0], y_SEPIN, T_SEPIN[0], F_SepLiquidOUT, Shell_T_Sep,
            F_SepVaporOUT, Working_Volume_separator[0], Total_Gas_Loop_Volume[0],
            UA_separator[0], VIJ, AIJ, MW, SpG, HVAP, HCAPLa, HCAPLb, HCAPVa, HCAPVb, A, B, C, R, Dw, T_ref);
    
    for (i=166;i<239;i++)
    {
        states_col[i-166] = states[i]; 
        dstatesdt_col[i-166]=0;
    }
    Column(dstatesdt_col, dstatesdt_other, T_Column, x_Organic, T_Organic, x_Aqueous, T_Aqueous, beta_out,
            states_col, P_Column, F_COLIN[0], x_COLIN, T_COLIN[0], F_Reflux,
            Q_Reboiler, F_Organic,F_Aqueous,F_Bottom, T_Column, F_HAc, x_HAc, T_HAc[0], F_Scrub, F_VapLiquidIn, 
            20, 15, P_top[0], P_bottom[0], T_decanter, K_decanter, hydtau_column[0], M0_column, L0_column,
            Bottom_Working_Level_Volume[0], Organic_Working_Level_Volume[0], Aqueous_Working_Level_Volume[0],
            HAcTank_Working_Level_Volume[0], VIJ, AIJ, MW, SpG, HVAP, HCAPLa, HCAPLb,HCAPVa, HCAPVb, A, B, C, R, Dw,
            T_ref, Q_Condenser);

    for (i=0;i<8;i++)
        *(dstatesdt+i) = dstatesdt_vap[i];
    for (i=8;i<78;i++)
        *(dstatesdt+i) = dstatesdt_rct[i-8];
    for (i=78;i<94;i++)
        *(dstatesdt+i) = dstatesdt_sep[i-78];
    for (i=94;i<166;i++)
        *(dstatesdt+i) = dstatesdt_abs[i-94];
    for (i=166;i<239;i++)
        *(dstatesdt+i) = dstatesdt_col[i-166];
    for (i=239;i<246;i++)
        *(dstatesdt+i) = dstatesdt_other[i-239];

    /*--------------------------------------------------------------------------------------*/
    measurements[0]=P_vaporizer[0];
    measurements[1]=hup_vaporizer[0];
    measurements[2]=T_vaporizer[0];
    measurements[3]=T_VapHeaterOut[0];
    measurements[4]=T_reactor[NR[0]-1];    
    measurements[5]=F_FEHEIN_hot[0];
    measurements[6]=T_FEHEColdOut[0];
    measurements[7]=T_SEPIN[0];
    measurements[8]=hup_separator[0];
    measurements[9]=TL_separator[0];
    measurements[10]=T_ABSIN_Gas[0];
    measurements[11]=P_recycle[0];
    measurements[12]=hup_base[0];
    measurements[13]=T_Circulation[0];
    measurements[14]=T_Scrub[0];
    measurements[15]=F_ToVap[0]+F_FEHEIN_cold[0];
    measurements[16]=F_Organic;
    measurements[17]=hup_organic[0];
    measurements[18]=hup_aqueous[0];
    measurements[19]=T_decanter[0];
    measurements[20]=hup_bottom[0];
    measurements[21]=T_Column[4];
    measurements[22]=hup_HAcTank[0];
    measurements[23]=x_Organic[4];
    measurements[24]=x_Organic[5];
    measurements[25]=x_Organic[6];
    measurements[26]=x_bottom[0];
    measurements[27]=x_bottom[1];
    measurements[28]=x_bottom[2];
    measurements[29]=y_ToVap[0];
    measurements[30]=y_ToVap[1];
    measurements[31]=y_ToVap[2];
    measurements[32]=y_ToVap[3];
    measurements[33]=y_ToVap[4];
    measurements[34]=y_ToVap[5];
    measurements[35]=y_ToVap[6];
    measurements[36]=y_RCTIN[0];
    measurements[37]=y_RCTIN[1];
    measurements[38]=y_RCTIN[2];
    measurements[39]=y_RCTIN[3];
    measurements[40]=y_RCTIN[4];
    measurements[41]=y_RCTIN[5];
    measurements[42]=y_RCTIN[6];

    for (i=0;i<26;i++)
    {
	    MVs_out[i] = MVs[i];
    }

	for (i=246;i<256;i++)
	{
        states[i] = dstatesdt_rct[i-176];
	}
    for (i=0;i<256;i++)
    {
       	states_out[i] = states[i];
    }
	
    //if disturbance 5
    if ((disturbance_5) && (is_initial!=1))
    {
        measurements[23]=0;
        measurements[24]=0;
        measurements[25]=0;
    }
    //if disturbance 6
    if ((disturbance_6) && (is_initial!=1))
    {
        measurements[26]=0;
        measurements[27]=0;
        measurements[28]=0;
    }
    //if disturbance 7
    if ((disturbance_7) && (is_initial!=1))
    {
        measurements[29]=0;
        measurements[30]=0;
        measurements[31]=0;
        measurements[32]=0;
        measurements[33]=0;
        measurements[34]=0;
        measurements[35]=0;
    }
#ifdef DEBUG
	debug("VAModel left.\n");
#endif
}

// #ifdef mex_h

// void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
// {

    // double *states;
    // double *MVs;
    // double time;
    // int is_initial;  
    // int *disturbances;  
    
    // int mrows;
    // int ncols;
  
    // double *dstatesdt;
    // double *states_out;
    // double *MVs_out;
    // double *measurements_full;  

    // if(nrhs!=5) 
        // mexErrMsgTxt("inputs not correct");
    // if(nlhs!=4) 
        // mexErrMsgTxt("outputs not correct");
  
    // states          = mxGetPr(prhs[0]);
    // MVs             = mxGetPr(prhs[1]);
    // time            = mxGetScalar(prhs[2]);
    // is_initial      = mxGetScalar(prhs[3]);
    // disturbances  	= mxGetPr(prhs[4]);
    
    // //-------------------------------------------
    // mrows = mxGetM(prhs[0]);
    // ncols = mxGetN(prhs[0]);
    // plhs[0] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    // dstatesdt = mxGetPr(plhs[0]);
    // //-------------------------------------------
    // mrows = mxGetM(prhs[0]);
    // ncols = mxGetN(prhs[0]);
    // plhs[1] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    // states_out = mxGetPr(plhs[1]);
    // //-------------------------------------------
    // mrows = mxGetM(prhs[1]);
    // ncols = mxGetN(prhs[1]);
    // plhs[2] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    // MVs_out = mxGetPr(plhs[2]);
    // //-------------------------------------------
    // mrows = 43;
    // ncols = 1;
    // plhs[3] = mxCreateDoubleMatrix(mrows,ncols, mxREAL);
    // measurements_full = mxGetPr(plhs[3]);

    // /*  Call the C subroutine. */
    // VAModel(dstatesdt, states_out, MVs_out, measurements_full, 
                // states, MVs, time, is_initial, disturbances);
// }

// #endif