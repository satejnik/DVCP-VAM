/*	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
 *	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	----------------------------------------------------------------------
 */	

#define S_FUNCTION_NAME  transmex
#define S_FUNCTION_LEVEL 2

#include <simstruc.h>
#include <simstruc_types.h>
#include "prototypes.h"
#include "debug.h"

#define NS 0
#define NO 2
#define DIMOUT1 1
#define DIMOUT2 DYNAMICALLY_SIZED
#define NI 2
#define DIMIN1 9
#define DIMIN2 DYNAMICALLY_SIZED
#define NPAR 1

/* Common Block Declarations */

#define SAMPLING_NPARAM 0
#define SAMPLING_PARAM(S) ssGetSFcnParam(S,SAMPLING_NPARAM)
#define SAMPLING_PARAM_DATA(S) mxGetPr(SAMPLING_PARAM(S))[0]

#define NINPUT 0
#define GETINPUT(S) *ssGetInputPortRealSignalPtrs(S,0)[NINPUT]
#define NDED 1
#define GETDED(S) *ssGetInputPortRealSignalPtrs(S,0)[NDED]
#define NTAU 2
#define GETTAU(S) *ssGetInputPortRealSignalPtrs(S,0)[NTAU]
#define NNDED 3
#define GETNDED(S) (int_T)*ssGetInputPortRealSignalPtrs(S,0)[NNDED]
#define NNTAU 4
#define GETNTAU(S) (int_T)*ssGetInputPortRealSignalPtrs(S,0)[NNTAU]
#define NHT 5
#define GETHT(S) *ssGetInputPortRealSignalPtrs(S,0)[NHT]
#define NLOWMEAS 6
#define GETLOWMEAS(S) *ssGetInputPortRealSignalPtrs(S,0)[NLOWMEAS]
#define NHIGHMEAS 7
#define GETHIGHMEAS(S) *ssGetInputPortRealSignalPtrs(S,0)[NHIGHMEAS]
#define NYYY 8
#define GETYYY(S) *ssGetInputPortRealSignalPtrs(S,0)[NYYY]

#define GETXXX(S) *ssGetInputPortRealSignalPtrs(S,1)

#define SETOUTPUT(S) ssGetOutputPortRealSignal(S,0)
#define SETXXX(S) ssGetOutputPortRealSignal(S,1)

static char msg[256];  /* For error messages */
static int_T code_sd;

#define COPY(target, cource, nx) {int i; for (i=0; i<nx; i++) {target[i] = cource[i];}}
#define INIT(target, nx) {int i; for (i=0; i<nx; i++) {target[i] = 0;}}

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
	//1 + deadtime * frequency;
#ifdef DEBUG
	debug("mdlInitializeSizes entered.\n");
	//debug("DED:%f\n", GETDED(S));
	//debug("HT:%f\n", GETHT(S));
	//debug("number:%f\n", 1 + GETDED(S) * (1 / GETHT(S)));
#endif
    ssSetNumSFcnParams(S, NPAR);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) return;
    }

    ssSetNumContStates(    S, NS);   /* number of continuous states           */
    ssSetNumDiscStates(    S, NS);   /* number of discrete states             */

    if (!ssSetNumInputPorts(S, NI)) return;
    ssSetInputPortWidth(S, 0, DIMIN1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
	ssSetInputPortWidth(S, 1, DIMIN2);
    ssSetInputPortDirectFeedThrough(S, 1, 1);

    if (!ssSetNumOutputPorts(S, NO)) return;
    ssSetOutputPortWidth(S, 0, DIMOUT1);
	ssSetOutputPortWidth(S, 1, DIMOUT2);

    ssSetNumSampleTimes(   S, 1);   /* number of sample times                */
    ssSetNumRWork(         S, 0);   /* number of real work vector elements   */
    ssSetNumIWork(         S, 0);   /* number of integer work vector elements*/
    ssSetNumPWork(         S, 0);   /* number of pointer work vector elements*/
    ssSetNumModes(         S, 0);   /* number of mode work vector elements   */
    ssSetNumNonsampledZCs( S, 0);   /* number of nonsampled zero crossings   */

    /* Take care when specifying exception free code - see sfuntmpl_doc.c */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
#ifdef DEBUG
	debug("mdlInitializeSizes left.\n");
#endif
} /* mdlInitializeSizes */

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_WIDTH
static void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T inputPortWidth)
{
#ifdef DEBUG
	debug("mdlSetInputPortWidth entered.\n");
#endif
	ssSetInputPortWidth(S,port,inputPortWidth);
	ssSetOutputPortWidth(S,port,inputPortWidth);
#ifdef DEBUG
	debug("mdlSetInputPortWidth left.\n");
#endif
} /* mdlSetInputPortWidth */

#define MDL_SET_OUTPUT_PORT_WIDTH
static void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T outputPortWidth)
{
#ifdef DEBUG
	debug("mdlSetOutputPortWidth entered.\n");
#endif
	ssSetInputPortWidth(S,port,outputPortWidth);
	ssSetOutputPortWidth(S,port,outputPortWidth);
#ifdef DEBUG
	debug("mdlSetOutputPortWidth left.\n");
#endif
} /* mdlSetOutputPortWidth */

# define MDL_SET_DEFAULT_PORT_DIMENSION_INFO
static void mdlSetDefaultPortDimensionInfo(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlSetDefaultPortDimensionInfo entered.\n");
#endif
	ssSetInputPortWidth(S, 1, 1);
	ssSetOutputPortWidth(S, 1, 1);
#ifdef DEBUG
	debug("mdlSetDefaultPortDimensionInfo left.\n");
#endif
} /* mdlSetDefaultPortDimensionInfo */
#endif

static void mdlInitializeSampleTimes(SimStruct *S)
{
	real_T sampling;
	sampling = SAMPLING_PARAM_DATA(S)/60;
#ifdef DEBUG
	debug("mdlInitializeSampleTimes entered.\n");
	//debug("Setting sampling time %f to %f.\n", SAMPLING_PARAM_DATA(S), sampling);
#endif
    ssSetSampleTime(S, 0, sampling);
    ssSetOffsetTime(S, 0, 0.0);
#ifdef DEBUG
	debug("mdlInitializeSampleTimes left.\n");
#endif
} /* mdlInitializeSampleTimes */

static void mdlOutputs(SimStruct *S, int_T tid)
{
	double y_original;
    double *x;
    double *y;
    double ded;
    double tau;
    int Nded;
    int Nlag;
    double h;
    double LO;
    double HI;
  
    double *x_out;
    double *y_out;
#ifdef DEBUG
	debug("mdlOutputs entered.\n");
#endif
	y_original = GETINPUT(S);
    x = GETXXX(S);
    y = &GETYYY(S);
    ded = GETDED(S);
    tau = GETTAU(S);
    Nded = GETNDED(S);
    Nlag = GETNTAU(S);
    h = GETHT(S);
    LO = GETLOWMEAS(S);
    HI = GETHIGHMEAS(S);
	
	x_out = SETXXX(S);
    y_out = SETOUTPUT(S);
	
	Transmit(x_out, y_out, y_original, x, y, ded, tau, Nded, Nlag, h, LO, HI);
	
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
}

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
