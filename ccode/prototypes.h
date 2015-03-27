/*	Copyright © 2015 Alexander Isakov. Contact: <alexander.isakov@tuhh.de>
 *	Copyright © 2015 Marina Krotofil. Contact: <marina.krotofil@tuhh.de>
 *	Copyright © 2015 TUHH-SVA Security in Distributed Applications.
 *	All rights reserved.
 *	License: http://opensource.org/licenses/BSD-3-Clause
 *	----------------------------------------------------------------------
 */	

/* Prototypes*/
#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__

#define NS 256
#define NM 26
#define NO 43
#define NPAR 1
#define ND 7
#define NC 8

//#define CONTINUOUS_STATES
#define DISCRETE_STATES

#if defined(CONTINUOUS_STATES)
#define STS(S) ssGetContStates(S)
#elif defined(DISCRETE_STATES)
#define STS(S) ssGetDiscStates(S)
#endif
#ifdef STS
#define STSI(S,index) STS(S)[index]
#endif

#define STATE_NPARAM 0
#define STATE_PARAM(S) ssGetSFcnParam(S,STATE_NPARAM)
#define STATE_PARAM_DATA(S) mxGetPr(STATE_PARAM(S))

#define COPY(target, source, nx) {int i; for (i=0; i<nx; i++) {target[i] = source[i];}}
#define INIT(target, nx) {int i; for (i=0; i<nx; i++) {target[i] = 0;}}
#define DIVIDE(source, nx, by) {int i; for (i=0; i<nx; i++) {source[i] = source[i]/by;}}
#define MULTIPLY(source, nx, by) {int i; for (i=0; i<nx; i++) {source[i] = source[i]*by;}}
#define SUM(target, source, nx) {int i; for (i=0; i<nx; i++) {target[i] = target[i] + source[i];}}

#define INPUT(S, index) (*ssGetInputPortRealSignalPtrs(S,index))
#define OUTPUT(S, index) ssGetOutputPortRealSignal(S,index)
#define SAMPLING(S, index) ssGetSampleTime(S, index)

#define GETTIME(S) ssGetT(S)
#define GETXMV(target, S) {int i; for (i=0; i<NM; i++) {target[i] = INPUT(S,0)[i];}}
#define GETIDV(target, S) {int i; for (i=0; i<ND; i++) {target[i] = INPUT(S,1)[i + NC];}}
#define GETCON(target, S) {int i; for (i=0; i<NC; i++) {target[i] = INPUT(S,1)[i];}}
#define DX(S) ssGetdX(S)
#define SETSTEADY(S) steadySTS(STS(S))

static real_T XMV[ NM ];
//#define XMV(S) INPUT(S,0)
static int_T IDV[ ND ];
//#define IDV(S) (INPUT(S,0)+sizeof(real_T)*NM)
//static real_T STATES[ NS ];

void report(const char* function, SimStruct *S);
void vacfunc(real_T *dx, real_T *xms, SimStruct *S);
void warning(const char *format , ... );

#if defined(__SIMSTRUC_TYPES_H__) && !defined(__SS_SimStatus_Strings__)
#define __SS_SimStatus_Strings__

SS_SimStatus ssGetSimState(SimStruct *S);

#define GetSimState(S) ssGetSimState(S)

time_T ssGetTime(SimStruct *S);

#define GetTime(S) ssGetTime(S)

static const char *SS_SimStatus_Strings[] = {
	"SIMSTATUS_STOPPED",
    "SIMSTATUS_UPDATING",
    "SIMSTATUS_INITIALIZING",
    "SIMSTATUS_RUNNING",
    "SIMSTATUS_PAUSED_IN_DEBUGGER",
    "SIMSTATUS_PAUSED",
    "SIMSTATUS_TERMINATING",
    "SIMSTATUS_EXTERNAL"};

#define SimState(S) SS_SimStatus_Strings[GetSimState(S)]

#endif

#endif /* __PROTOTYPES_H__ */