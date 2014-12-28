/*	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
 * 	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	---------------------------------------------------------------------
 */	

#define S_FUNCTION_NAME  contrmex
#define S_FUNCTION_LEVEL 2

#include <simstruc.h>
#include <simstruc_types.h>
#include "prototypes.h"
#include "debug.h"

#define NS 0
#define NO 3
#define NI 18
#define NPAR 1

/* Common Block Declarations */

#define SAMPLING_NPARAM 0
#define SAMPLING_PARAM(S) ssGetSFcnParam(S,SAMPLING_NPARAM)
#define SAMPLING_PARAM_DATA(S) mxGetPr(SAMPLING_PARAM(S))[0]

#define NINPUT 0
#define INPUT(S) *ssGetInputPortRealSignalPtrs(S,0)[NINPUT]
#define NSETPOINT 1
#define SETPOINT(S) *ssGetInputPortRealSignalPtrs(S,0)[NSETPOINT]
#define NMODE 2
#define MODE(S) (int_T)*ssGetInputPortRealSignalPtrs(S,0)[NMODE]
#define NGAIN 3
#define GAIN(S) *ssGetInputPortRealSignalPtrs(S,0)[NGAIN]
#define NRESET 4
#define RESET(S) *ssGetInputPortRealSignalPtrs(S,0)[NRESET]
#define NSAMPLING 5
#define SAMPLING(S) *ssGetInputPortRealSignalPtrs(S,0)[NSAMPLING]
#define NLOWMEAS 6
#define LOWMEAS(S) *ssGetInputPortRealSignalPtrs(S,0)[NLOWMEAS]
#define NHIGHMEAS 7
#define HIGHMEAS(S) *ssGetInputPortRealSignalPtrs(S,0)[NHIGHMEAS]
#define NLOWMV 8
#define LOWMV(S) *ssGetInputPortRealSignalPtrs(S,0)[NLOWMV]
#define NHIGHMV 9
#define HIGHMV(S) *ssGetInputPortRealSignalPtrs(S,0)[NHIGHMV]
#define NPGAIN 10
#define PGAIN(S) (int_T)*ssGetInputPortRealSignalPtrs(S,0)[NPGAIN]
#define NPONLY 11
#define PONLY(S) (int_T)*ssGetInputPortRealSignalPtrs(S,0)[NPONLY]
#define NRELLO 12
#define RELLO(S) *ssGetInputPortRealSignalPtrs(S,0)[NRELLO]
#define NRELHI 13
#define RELHI(S) *ssGetInputPortRealSignalPtrs(S,0)[NRELHI]
#define NHYS 14
#define HYS(S) *ssGetInputPortRealSignalPtrs(S,0)[NHYS]
#define NHIFLAG 15
#define HIFLAG(S) *ssGetInputPortRealSignalPtrs(S,0)[NHIFLAG]
#define NMV 16
#define MV(S) *ssGetInputPortRealSignalPtrs(S,0)[NMV]
#define NXI 17
#define XI(S) *ssGetInputPortRealSignalPtrs(S,0)[NXI]

#define NOUTPUT 0
#define OUTPUT(S) ssGetOutputPortRealSignal(S,0)[NOUTPUT]
#define NXO 1
#define XO(S) ssGetOutputPortRealSignal(S,0)[NXO]
#define NFLAG 2
#define FLAG(S) ssGetOutputPortRealSignal(S,0)[NFLAG]

static char msg[256];  /* For error messages */
static int_T code_sd;

#define COPY(target, cource, nx) {int i; for (i=0; i<nx; i++) {target[i] = cource[i];}}
#define INIT(target, nx) {int i; for (i=0; i<nx; i++) {target[i] = 0;}}

/*====================*
 * S-function methods *
 *====================*/

/*====================================================================*
 * Parameter handling methods. These methods are not supported by RTW *
 *====================================================================*/

#define MDL_CHECK_PARAMETERS   /* Change to #undef to remove function */
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
static void mdlCheckParameters(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlCheckParameters entered.\n");
#endif
	//TODO
#ifdef DEBUG
	debug("mdlCheckParameters left.\n");
#endif
} /* mdlCheckParameters */
#endif 

/*=====================================*
 * Configuration and execution methods *
 *=====================================*/
static void mdlInitializeSizes(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlInitializeSizes entered.\n");
#endif
    ssSetNumSFcnParams(S, NPAR);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) return;
    }

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, NI);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, NO);

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Take care when specifying exception free code - see sfuntmpl_doc.c */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
#ifdef DEBUG
	debug("mdlInitializeSizes left.\n");
#endif
} /* mdlInitializeSizes */

static void mdlInitializeSampleTimes(SimStruct *S)
{
	real_T sampling;	
#ifdef DEBUG	
	debug("mdlInitializeSampleTimes entered.\n");
#endif
	sampling = SAMPLING_PARAM_DATA(S)/60;
    ssSetSampleTime(S, 0, sampling);
    ssSetOffsetTime(S, 0, 0.0);
#ifdef DEBUG
	debug("mdlInitializeSampleTimes left.\n");
#endif
} /* mdlInitializeSampleTimes */

static void mdlOutputs(SimStruct *S, int_T tid)
{
	double sp_original;
    double meas_scaled;
    double manip_original_in;
    double xi_in;
    int mode;
    double K;
    double Ti;
    double h;
    double LO_y;
    double HI_y;
    double LO_u;
    double HI_u;
    int action;
    int P_only;
    double rel_lo;
    double rel_hi;
    double hys;
    double hi_flag_in;

    double *manip_original;
    double *xi;
    double *hi_flag;
#ifdef DEBUG
	debug("mdlOutputs entered.\n");
#endif
	sp_original = SETPOINT(S);
	meas_scaled = INPUT(S);
	manip_original_in = MV(S);
	xi_in = XI(S);
	mode = MODE(S);
	K = GAIN(S);
	Ti = RESET(S);
	h = SAMPLING(S);
	LO_y = LOWMEAS(S);
	HI_y = HIGHMEAS(S);
	LO_u = LOWMV(S);
	HI_u = HIGHMV(S);
	action = PGAIN(S);
	P_only = PONLY(S);
	rel_lo = RELLO(S);
	rel_hi = RELHI(S);
	hys = HYS(S);
	hi_flag_in = HIFLAG(S);
	
	manip_original = &OUTPUT(S);
	xi = &XO(S);
	hi_flag = &FLAG(S);
	
	controller(manip_original, xi, hi_flag, sp_original, meas_scaled, manip_original_in, xi_in, mode, K, Ti, h, LO_y, HI_y, LO_u, HI_u, action, P_only, rel_lo, rel_hi, hys, hi_flag_in);

#ifdef DEBUG
	debug("mdlOutputs left.\n");
#endif
} /* mdlOutputs */

static void mdlTerminate(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlTerminate entered.\n");
#endif
	if (code_sd != (int_T) 0 ) {
		mexWarnMsgTxt(msg);
	}
#ifdef DEBUG
	debug("mdlTerminate left.\n");
#endif
} /* mdlTerminate */

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
