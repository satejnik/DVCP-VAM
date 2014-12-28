/*	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
 * 	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	---------------------------------------------------------------------
 */	

// Level-2 c-mex S function interface for the VA problem.
// Number discrete states = 246
// Number of inputs = 26 manipulated, 7 disturbances / 33
// Number of outputs = 43
// Number of parameters = 1

#define S_FUNCTION_NAME vamex
#define S_FUNCTION_LEVEL 2

#include <simstruc.h>
#include <simstruc_types.h>
#include "prototypes.h"
#include "debug.h"
 
static char msg[256];  /* For error messages */
static int_T code_sd;

/*====================*
 * S-function methods *
 *====================*/

#define MDL_CHECK_PARAMETERS   /* Change to #undef to remove function */
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)

static void mdlCheckParameters(SimStruct *S)
{
	size_t nelms;
#ifdef DEBUG
	debug("mdlCheckParameters entered.\n");
#endif
	if (!mxIsEmpty(STATE_PARAM(S)))
	{
		if (mxIsSparse(STATE_PARAM(S)) ||
			mxIsComplex(STATE_PARAM(S)) ||
			!mxIsNumeric(STATE_PARAM(S)))
		{
			sprintf(msg,"Error in parameter %i:  %s",STATE_NPARAM+1,
				"Parameter must be a real, non-sparse vector.");
			ssSetErrorStatus(S,msg);
			return;
		}
		nelms = mxGetNumberOfElements(STATE_PARAM(S));
		if (nelms != NS)
		{
			sprintf(msg,"Error in parameter %i:  Length = %i"
				".  Expecting length = %i", STATE_NPARAM+1, nelms, NS);
			ssSetErrorStatus(S,msg);
			return;
		}
	}
#ifdef DEBUG
	debug("mdlCheckParameters left.\n");
#endif
} /* mdlCheckParameters */
#endif 

static void mdlInitializeSizes(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlInitializeSizes entered.\n");
#endif
    ssSetNumSFcnParams(S, NPAR);  /* Number of expected parameters */
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
		if (ssGetErrorStatus(S) != NULL) return;
    }	
	else return;     /*Simulink will report a parameter mismatch error */

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, NS);

    if (!ssSetNumInputPorts(S, 1)) return;
	ssSetInputPortWidth(S, 0, NM + ND);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 2)) return;
	ssSetOutputPortWidth(S, 0, NO);
	ssSetOutputPortWidth(S, 1, NS);

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
#ifdef DEBUG
	debug("mdlInitializeSizes left.\n");
#endif 
} /* mdlInitializeSizes */

static void mdlInitializeSampleTimes(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlInitializeSampleTimes entered.\n");
#endif
	ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
#ifdef DEBUG
	debug("mdlInitializeSampleTimes left.\n");
#endif	
} /* mdlInitializeSampleTimes */

#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlInitializeConditions entered.\n");
#endif
	if (ssIsFirstInitCond(S)) steadySTS(STS(S));
	if (!mxIsEmpty(STATE_PARAM(S))) COPY(STS(S), STATE_PARAM_DATA(S), NS);
#ifdef DEBUG
	debug("mdlInitializeConditions left.\n");
#endif
} /* mdlInitializeConditions */

static void mdlOutputs(SimStruct *S, int_T tid)
{
	real_T dx[NS];
#ifdef DEBUG
	debug("mdlOutputs entered.\n");
#endif
    UNUSED_ARG(tid); /* not used in single tasking mode */
	vacfunc(dx, OUTPUT(S,0), S);
	COPY(OUTPUT(S,1),STS(S),NS);
	SUM(STS(S), dx, NS);
#ifdef DEBUG
	debug("mdlOutputs left.\n");
#endif
} /* mdlOutputs */

static void mdlTerminate(SimStruct *S)
{
#ifdef DEBUG
	debug("mdlTerminate entered.\n");
#endif
    UNUSED_ARG(S); /* unused input argument */
	if (code_sd != (int_T) 0 ) {
		mexWarnMsgTxt(msg);
	}
#ifdef DEBUG
	debug("mdlTerminate left.\n");
#endif
} /* mdlTerminate */

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
