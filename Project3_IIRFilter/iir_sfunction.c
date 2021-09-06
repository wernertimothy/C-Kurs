#undef S_FUNCTION_NAME
#define S_FUNCTION_NAME iir_sfunction       // s function name
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#define A_COEFFS ssGetSFcnParam(S,0)
#define B_COEFFS ssGetSFcnParam(S,1)

static void mdlInitializeSizes(SimStruct *S)
{  
    ssSetNumSFcnParams(S, 2);               // nom coeffs, denom coeffs
    
    ssSetNumContStates(S, 0);               // none
    ssSetNumDiscStates(S, 0);               // none

	ssSetNumRWork(S, 0);                    // none
    ssSetNumIWork(S, 2);                    // filter order n, current index j in buffer
    ssSetNumPWork(S, 2);                    // pointer to y_buffer, pointer to u_buffer 
    
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
    // define filter variables
    int n = mxGetNumberOfElements(A_COEFFS) - 1;                // fitler order
    real_T* u_buffer = (real_T*) malloc(n*sizeof(real_T));      // buffers u[k-i], i\in[1,n]
    real_T* y_buffer = (real_T*) malloc(n*sizeof(real_T));      // buffers y[k-i], i\in[1,n]
    int j = 0;      // curent index in buffer
    
    // initialize buffer to zero
    memset(u_buffer, 0, n*sizeof(real_T));
    memset(y_buffer, 0, n*sizeof(real_T));
    
    // save stuff in work array
    ssSetIWorkValue(S, 0, n);           // first element is filter order
    ssSetIWorkValue(S, 1, j);           // first element is current buffer index
    
    ssSetPWorkValue(S, 0, u_buffer);    // first element is u_buffer
    ssSetPWorkValue(S, 1, y_buffer);    // second element is y_buffer       
}
#endif


static void mdlOutputs(SimStruct *S, int_T tid)
{
   // set work values
   int n            = ssGetIWorkValue(S, 0);
   int j            = ssGetIWorkValue(S, 1);
   int l            = 0;
   real_T* u_buffer = (real_T*)ssGetPWorkValue(S, 0);
   real_T* y_buffer = (real_T*)ssGetPWorkValue(S, 1);
   
   // set parameter values
   real_T* a = (real_T*) mxGetData(A_COEFFS);
   real_T* b = (real_T*) mxGetData(B_COEFFS);
   
   // get input data
   InputRealPtrsType u = (InputRealPtrsType) ssGetInputPortRealSignalPtrs(S, 0);
   real_T uk           = (*u)[0];
   
   // set output variable
   real_T* y = (real_T*) ssGetOutputPortRealSignal(S, 0);
   real_T yk = 0.0;
   
   // calculate filter output
   // filter equation : a[0]*y[k] = b[0]*u[k] + sum_{i = 1}^n b[i]*u[k-1] - a[i]*y[k-i]
   yk = b[0]*uk;
   for (int i = 1; i < n+1; i++) {
       l = j - i;
       if (l < 0) {l += n;};
       yk += b[i]*u_buffer[l] - a[i]*y_buffer[l];
   }
   // write output
   y[0] = yk;
   
   // save last n data in buffer
   u_buffer[j] = uk;
   y_buffer[j] = yk;
   // update buffer index
   j ++;
   j = j%n;
   
   // save stuff in work array
   ssSetIWorkValue(S, 1, j);           // first element is current buffer index
   ssSetPWorkValue(S, 0, u_buffer);    // first element is u_buffer
   ssSetPWorkValue(S, 1, y_buffer);    // second element is y_buff
}
  

static void mdlTerminate(SimStruct *S)
{
   // free allocated work memory
   real_T* u_buffer = (real_T*)ssGetPWorkValue(S, 0);
   real_T* y_buffer = (real_T*)ssGetPWorkValue(S, 1);
   free(u_buffer);
   free(y_buffer);
   int* Nullptr = (int*)0;
   ssSetPWorkValue(S, 0, Nullptr);
   ssSetPWorkValue(S, 1, Nullptr);
}

/* S-Function trailer
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else                  
#include "cg_sfun.h"       /* Code generation registration function */
#endif                 
