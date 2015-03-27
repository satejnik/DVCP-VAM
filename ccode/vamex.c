/*	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
 *	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	----------------------------------------------------------------------
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
#include <string.h>
#include "prototypes.h"
#include "debug.h"
 
static char msg[100];  /* For error messages */
static int_T simErrCode;
static int_T simWarnFlags[10];

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

    if (!ssSetNumInputPorts(S, 2)) return;
	ssSetInputPortWidth(S, 0, NM);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
	ssSetInputPortWidth(S, 1, NC + ND);
    ssSetInputPortDirectFeedThrough(S, 1, 1);

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
	simErrCode = (int_T) 0;
	INIT(simWarnFlags, 10);
	*msg = '\0';
#ifdef DEBUG
	debug("mdlInitializeConditions left.\n");
#endif
} /* mdlInitializeConditions */

static void mdlOutputs(SimStruct *S, int_T tid)
{
	real_T dx[NS], cons[NC];
	int_T i;
#ifdef DEBUG
	debug("mdlOutputs entered.\n");
#endif
    UNUSED_ARG(tid); /* not used in single tasking mode */

#ifdef DEBUG
	if (ssGetT(S) >= 0.316944) {
		ssPrintf("(%f)", ssGetT(S));
	}
#endif
	
	
	// operating loop
	vacfunc(dx, OUTPUT(S,0), S);
	COPY(OUTPUT(S,1),STS(S),NS);
	SUM(STS(S), dx, NS-10); // ten last are not derivatives
	//printf("%f\n", ssGetT(S));
	/* constraints check */
	GETCON(cons, S);
	// Liquid levels in the vaporizer, separator, absorber base, column base, and decanter must operate
	// within the limits of 10-90%.
	if (cons[3]) {
		int_T inds[6] = {1, 8, 12, 20, 17, 18};
		char *tanks[6] = {"Vaporizer","Separator","Absorber","Column","Decanter Org.","Decanter Aq."};
		for (i=0;i<6;i++) {
			if (OUTPUT(S,0)[inds[i]] < .10 && !simWarnFlags[i]) {
				//mexWarnMsgIdAndTxt("VAcModel:output:constraint:4", "Liquid Level in %s underran 10%.\n", tanks[i]);
				warning("(%f): Liquid Level in %s underran 10%%", ssGetT(S), tanks[i]);
				simWarnFlags[i] = 1;
			} 
			else if (OUTPUT(S,0)[inds[i]] > .90 && !simWarnFlags[i]) {
				//mexWarnMsgIdAndTxt("VAcModel:output:constraint:4", "Liquid Level in %s exceeded 90%.\n", tanks[i]);
				warning("(%f): Liquid Level in %s exceeded 90%%", ssGetT(S), tanks[i]);
				simWarnFlags[i] = 1;
			}
			else if (simWarnFlags[i]) simWarnFlags[i] = 0;
		}
	}
	// Reactor feed temperature must be greater than 130 oC.
	if (cons[4]) {
		if (STS(S)[68] < 130 && !simWarnFlags[6]) {
			//mexWarnMsgIdAndTxt("VAcModel:output:constraint:5", "(%f): Reactor Feed Temp underran 130C.", ssGetT(S));
			warning("(%f): Reactor Feed Temp underran 130C", ssGetT(S));
			simWarnFlags[6] = 1;
		} else if (simWarnFlags[6]) simWarnFlags[6] = 0;
	}
	// FEHE hot effluent temperature must be greater than 130 oC.
	if (cons[5]) {
		if (OUTPUT(S,0)[7] < 130 && !simWarnFlags[7]) {
			//mexWarnMsgIdAndTxt("VAcModel:output:constraint:6", "(%f): FEHE hot effluent temp underran 130C.", ssGetT(S));
			warning("(%f): FEHE hot effluent temp underran 130C", ssGetT(S));
			simWarnFlags[7] = 1;
		} else if (simWarnFlags[7]) simWarnFlags[7] = 0;
	}
	// The HAc composition must not exceed 0.06 mol% in the decanter organic phase.
	if (cons[6]) {
		if (OUTPUT(S,0)[25] > .0006 && !simWarnFlags[8]) {
			//mexWarnMsgIdAndTxt("VAcModel:output:constraint:7", "HAc comp. in decanter org. phase exceeded 0.06 Mol%.\n");
			warning("(%f): HAc comp. in decanter org. phase exceeded 0.06 Mol%%", ssGetT(S));
			simWarnFlags[8] = 1;
		} else if (simWarnFlags[8]) simWarnFlags[8] = 0;
	}
	// The VAc composition must not exceed 0.01 mol% in the column bottom.
	if (cons[7]) {
		if (OUTPUT(S,0)[26] > .0001 && !simWarnFlags[9]) {
			//mexWarnMsgIdAndTxt("VAcModel:output:constraint:8", "VAC comp. in column bottom exceeded 0.01 Mol%.\n");
			warning("(%f): VAC comp. in column bottom exceeded 0.01 Mol%%", ssGetT(S));
			simWarnFlags[9] = 1;
		} else if (simWarnFlags[9]) simWarnFlags[9] = 0;
	}
	
	//The O2 composition must not exceed 8 mol% anywhere in the gas recycle loop.
	if (cons[0] && OUTPUT(S,0)[29] > .08) {
		simErrCode = (int_T) 1;
		warning("(%f): Gas Loop O2 Composition exceeded 0.08 Mol%%", ssGetT(S));
		warning("Shutting down..");
		ssSetStopRequested(S,1);
		return;
	}
	//The pressure in the gas recycle loop must not exceed 140 psia.
	if (OUTPUT(S,0)[11] > 140.0) {
		simErrCode = (int_T) 2;
		warning("(%f): GL Pressure exceeded 140 psia", ssGetT(S));
		warning("Shutting down..");
		ssSetStopRequested(S,1);
		return;
	}
	//The peak reactor temperature along the length of the tube must not exceed 200C.
	if (cons[2]) {
		int_T flag = 0; 
		for (i=68;i<77;i++)
			if (STS(S)[i] > 200.0) {
				warning("(%f): Reactor Temp exceeded 200C; Section %d", ssGetT(S), i-67);
				if (!flag) flag!=flag;
			}
		if (flag) {
			simErrCode = (int_T) 3;
			warning("Shutting down..");
			ssSetStopRequested(S,1);
			return;
		}
	}
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
#ifdef DEBUG
	debug("mdlTerminate left.\n");
#endif
} /* mdlTerminate */

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
