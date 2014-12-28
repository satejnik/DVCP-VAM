/*	Copyright © 2014 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2014 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2014 TUHH-SVA Security in Distributed Applications.
 * 	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	---------------------------------------------------------------------
 */	

#include <simstruc.h>
#include <simstruc_types.h>
#include "prototypes.h"
#include "debug.h"

#ifdef __PROTOTYPES_H__

void report(const char* message, SimStruct *S)
{
	ssPrintf("%f: %s (%s).\n", ssGetTime(S), message, SimState(S));
}

#ifdef __SS_SimStatus_Strings__

time_T ssGetTime(SimStruct *S)
{
	time_T time = 0.0;
	if(GetSimState(S) == SIMSTATUS_RUNNING)
		time = ssGetT(S);
	return time;
	
	// SIMSTATUS_STOPPED,
    // SIMSTATUS_UPDATING,
    // SIMSTATUS_INITIALIZING,
    // SIMSTATUS_RUNNING,
    // SIMSTATUS_PAUSED_IN_DEBUGGER,
    // SIMSTATUS_PAUSED,
    // SIMSTATUS_TERMINATING,

    // /* Must be last */
    // SIMSTATUS_EXTERNAL
}

int ssGetSimState(SimStruct *S)
{
	SS_SimStatus status;	
	ssGetSimStatus(S, &status);
	return status;
}

#endif /* __SS_SimStatus_Strings__ */

void vacfunc(real_T *dx, real_T *xms, SimStruct *S)
{
	real_T mvs[NM], x[NS];
#ifdef DEBUG
	debug("vacfunc entered.\n");
#endif
// #ifdef DEBUG
	// debug("Time:%f (in minutes: %f) entered.\n",GETTIME(S), GETTIME(S)*60);
// #endif
	GETXMV(XMV,S);
	GETIDV(IDV,S);
	VAModel(dx, x, mvs, xms, STS(S), XMV, GETTIME(S)*60, 0, IDV);
#if defined(CONTINUOUS_STATES)
	MULTIPLY(dx, NS, 60);
#elif defined(DISCRETE_STATES)
	DIVIDE(dx, NS, 180);
#endif
#ifdef DEBUG
	debug("vacfunc left.\n");
#endif
}

#endif /* __PROTOTYPES_H__ */