#undef S_FUNCTION_NAME
#define S_FUNCTION_NAME pid_sfunction       // s function name
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

static void mdlInitializeSizes(SimStruct *S)
{  
    // PID inputs
    ssSetNumSFcnParams(S, 6);               // Kp, Kd, Ki, u0, umin, umax
    
    ssSetNumContStates(S, 0);               // none
    ssSetNumDiscStates(S, 0);               // none

	ssSetNumRWork(S, 2);                    // error_integral, error_old
    ssSetNumIWork(S, 0);                    // none
    ssSetNumPWork(S, 0);                    // none
    
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
     
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);

    ssSetInputPortDataType(S, 0, SS_DOUBLE);
    ssSetOutputPortDataType(S, 0, SS_DOUBLE); 
    
    ssSetNumSampleTimes(S, 1);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
}


static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);   // user specified sample rate
    ssSetOffsetTime(S, 0, 0.0);
}


#define MDL_START 
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{
    // define PID workspace variables
    real_T error_old = 0.0;           // Fehler aus letzen Zeitschritt
    real_T error_integral = 0.0;      // Integral des Fehlers

    ssSetRWorkValue(S, 0, error_old);
    ssSetRWorkValue(S, 1, error_integral);
}
#endif


static void mdlOutputs(SimStruct *S, int_T tid)
{
   // set work values
   real_T error_old = ssGetRWorkValue(S, 0);
   real_T error_integral = ssGetRWorkValue(S, 1);
   real_T dt = 0.01;

   // set parameter values
   real_T* Kp   = (real_T*) mxGetData(ssGetSFcnParam(S,0));
   real_T* Kd   = (real_T*) mxGetData(ssGetSFcnParam(S,1));
   real_T* Ki   = (real_T*) mxGetData(ssGetSFcnParam(S,2));
   real_T* u0   = (real_T*) mxGetData(ssGetSFcnParam(S,3));
   real_T* umin = (real_T*) mxGetData(ssGetSFcnParam(S,4));
   real_T* umax = (real_T*) mxGetData(ssGetSFcnParam(S,5));
   
   // get input data
   InputRealPtrsType u = (InputRealPtrsType) ssGetInputPortRealSignalPtrs(S, 0);
   real_T error        = (*u)[0];
   
   // set output variable
   real_T* u_pid = (real_T*) ssGetOutputPortRealSignal(S, 0);

    // calculate derivative
    real_T error_derivative = ( error - error_old )/dt;

    // calculate PID action
    real_T dU = (*Kp)*error + (*Ki)*error_integral + (*Kd)*error_derivative + (*u0);
    u_pid[0] = dU;

    // saturation and anti-windup
    if (dU < (*umin)) {
        // pid action is less than actuator minimum
        dU = (*umin);
        // stop integrating
        ssSetRWorkValue(S, 1, error_integral);
    } else if (dU > (*umax)) {
        // pid action is more than actuator maximum
        dU = (*umax);
        // stop integrating
        ssSetRWorkValue(S, 1, error_integral);
    } else {
        ssSetRWorkValue(S, 1, error_integral + error*dt);
    }

   // save error in work array
   ssSetRWorkValue(S, 0, error);
}
  

static void mdlTerminate(SimStruct *S)
{
   // free allocated work memory
   ssSetRWorkValue(S, 0, 0.0);
   ssSetRWorkValue(S, 1, 0.0);
 
}

/* S-Function trailer
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else                  
#include "cg_sfun.h"       /* Code generation registration function */
#endif                 
