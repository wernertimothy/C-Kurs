#undef S_FUNCTION_NAME 
#define S_FUNCTION_NAME fir_filter_sfunction_v2        /* Name der S-Function festlegen */
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"       /* wegen SimStruct S */

#define COEFF_VECT ssGetSFcnParam(S,0)

static void mdlInitializeSizes(SimStruct *S)
{
    /*  In der Funktion mdlInitializeSizes müssen wir Simulink
     *  die Schnittstellen unseres Blockes mitteilen.
     *  D.h. wir sagen wieviele Ein- und Ausgänge wir haben,
     *  welche Dimensionen diese Ports haben, welche Daten Simulink 
     *  für uns zwischen mehreren Zeitschritten 
     *  speichern muss, ob wir Parameter erwarten, usw.
     */
    
    /* Diese S-Function hat einen Parameter. (Filterkoeffizienten b)
     */
    ssSetNumSFcnParams(S, 1);  
    
    /* Wir verwenden an dieser Stelle keine Simulink Zustaende,
     * sondern nutzen dafuer ein anderes Interface (s.u.).
     * Kontinuierliche Zustaende werden von einem DGL-Solver
     * geloest werden, dx/dt = f(x,u), und diskrete Zustaende
     * ueber Differenzengleichungen x_neu = f(x,u).
     */
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    /* Simulink bietet uns an, zusaetzlich zu den Zustaenden noch
     * je einen Vektor mit Gleitkommazahlen (RWork), Integers (IWork)
     * und Pointer (PWork) zu speichern.
     * Wir muessen hier nur einen Pointer auf eine Struktur speichern:
     */
	ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 1);    // aktueller Index im Ringpuffer
    ssSetNumPWork(S, 1);    // Zeiger auf ringpuffer
    
    /* Unser Block hat zwei Eingaenge, naemlich das zu filternde Signal
     * sowie den Koeffizientenvektor. Das Eingangssignal muss skalar sein,
     * der Koeffizientenvektor kann beliebiger Groesse sein.
     * Beide Eingaenge haben direkte Wirkung auf die Ausgaenge,
     * d.h. sie werden in der mdlOutputs-Funktion verwendet.
     */
    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    /* Unser Block hat nur einen Ausgang, naemlich das gefilterte Signal.
     * Es ist skalar.
     */
    if (!ssSetNumOutputPorts(S, 1)) return;
    ssSetOutputPortWidth(S, 0, 1);

    /* Diese S-Function soll in Double (doppelte Genauigkeit) rechnen.
     */
    ssSetInputPortDataType(S, 0, SS_DOUBLE);
    ssSetOutputPortDataType(S, 0, SS_DOUBLE); 
    
    /* Unser Block hat nur eine Abtastrate
     */
    ssSetNumSampleTimes(S, 1);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

}


static void mdlInitializeSampleTimes(SimStruct *S)
{
    /* Mit der Funktion mdlInitializeSampleTimes teilen wir
     * Simulink mit, mit welcher Abtastrate unser Block ausgeführt werden 
     * soll. 
     * Hier gehen wir davon aus, dass der Simulink-Programmierer
     * das von oben her im Griff hat, d.h. wir erben einfach eine uns
     * zugewiesene Abtastrate (INHERITED_SAMPLE_TIME).
     */
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
}


#define MDL_START 
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{
   /* Die mdlStart wird einmalig zu Simulationsbeginn ausgeführt.
    * Dies ist ein sinnvoller Ort, um dynamisch Speicher zu reservieren.
    */
   
    /* Verwendete Variablen
     */
   int N;
   int speicherbedarf_in_bytes;
   real_T *ringpuffer;
   
   /* Zunaechst wird ausgelesen, wie lang das Filter ist.
    */
   N = mxGetNumberOfElements(COEFF_VECT) - 1;
   
   /* Reserviere Speicher fuer Ringpuffer
    */
   speicherbedarf_in_bytes = sizeof(real_T)*N;
   ringpuffer = (real_T *) malloc(speicherbedarf_in_bytes);
   
   if (ringpuffer == 0)
   {
       /* Speicherreservierung fehlgeschlagen. Beende Simulation
        */
       ssSetErrorStatus(S, "Speicherreservierung fehlgeschlagen.");
       return;
   }
   
   /* Initialisiere Ringpuffer zu Null. (Filter wird mit Zustand Null initialisiert)
    */
   memset(ringpuffer, 0, speicherbedarf_in_bytes);
   
   ssSetPWorkValue(S, 0, ringpuffer);
   ssSetIWorkValue(S, 0, 0);
}
#endif

static void mdlOutputs(SimStruct *S, int_T tid)
{
   /* Die mdlOutputs wird in jedem Simulink-Zeitschritt ausgeführt.
    * Hier müssen aus den Eingangsdaten des S-Function-Blocks neue 
    * Ausgangsdaten berechnet werden.
    */
   
   /* Verwendete Variablen
    */
   real_T *y;
   InputRealPtrsType u;
   real_T *koeffizienten;
   real_T eingangssignal;
   real_T ausgangssignal;
   real_T *ringpuffer;
   int N;   // Filterlaenge
   int i;
   int k;   // Index naechstes zu schreibendes Element
   int ringpufferindex;
   
   /* Zunaechst werden Pointer auf unsere Block Ein- und Ausgaenge
    * ausgelesen. Dazu stellt SImulink Funktionen bereit.
    */
   y = (real_T *) ssGetOutputPortRealSignal(S, 0);
   u = (InputRealPtrsType) ssGetInputPortRealSignalPtrs(S, 0);
   koeffizienten = (real_T *) mxGetData(COEFF_VECT);
   
   /* Pointer auf vorab reservierten Speicher auslesen,
    * und Index des naechsten zu beschreibenden Elementes im Ringpuffer.
    */
   ringpuffer = (real_T *)ssGetPWorkValue(S, 0);
   k = ssGetIWorkValue(S, 0);
   
   /* Filterlaenge einlesen
    */
   N = mxGetNumberOfElements(COEFF_VECT) - 1;
   
   /* Filterberechnung
    * Eingangsignal einlesen und in Ringpuffer schreiben.
    */
   eingangssignal = (*u)[0];
   ringpuffer[k] = eingangssignal;
   
   /* Da ein Element in den Ringpuffer hineingeschrieben wurde, wird
    * nun k inkrementiert und gespeichert.
    * Dabei wird k umgebrochen, d.h. bei k==N wird k=0 gesetzt.
    */
   k++;
   k = k%N;
   ssSetIWorkValue(S, 0, k);
   
   /* Ausgang berechnen
    */
   ausgangssignal = 0.0;
   for (i=0; i < N; i++)
   {
       ringpufferindex = k-1-i;
       if (ringpufferindex < 0) ringpufferindex = ringpufferindex + N;
       ausgangssignal += koeffizienten[i] * ringpuffer[ringpufferindex];
   }
   y[0] = ausgangssignal;
}
     
static void mdlTerminate(SimStruct *S)
{
   /* Die Funktion mdlTerminate wird einmalig beim Beenden einer Simulation
    * ausgeführt. Dies ist ein sinnvoller Ort um den in der mdlStart 
    * reservierten Speicher wieder freizugeben.
    */
    
    /* In dieser Funktion verwendete Variablen
     */
    real_T *ringpuffer;
    
    /* Pointer auf vorab reservierten Speicher auslesen
     */
    ringpuffer = (real_T *)ssGetPWorkValue(S, 0);
    
    /* Speicher freigeben
     */
    free(ringpuffer);
    
    /* Sicherheitshalber die gespeicherten Pointer wieder auf Null setzen,
     * damit falls jemand anders sie noch ausliest, er weiss, dass dahinter
     * kein reservierter Speicher mehr liegt.
     */
    int * Nullpointer;
    Nullpointer = (int *) 0;
    ssSetPWorkValue(S, 0, Nullpointer);
}

/* S-Function trailer
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else                  
#include "cg_sfun.h"       /* Code generation registration function */
#endif                 
