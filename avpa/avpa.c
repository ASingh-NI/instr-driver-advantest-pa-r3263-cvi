#include <formatio.h>
#include <visa.h>
#include <stdlib.h>  
#include <stdio.h>  
#include <math.h>
#include "AVPA.h"

#define AVPA_REVISION  "Rev 2.0, 05/99, CVI 5.0.1" /*Instrument driver revision*/
#define BUFFER_SIZE    512L              /* File I/O buffer size */

/*= ADVANTEST PA Series Spectrum Analyzers R3263, R3272, R3463 and R3465 ====*/
/*                                                                           */
/* LabWindows/CVI 5.0.1 Instrument Driver                                    */
/* Original Release: February 1998                                           */
/* By: Denis BIRINGER, Advantest Europe R&D S.A.R.L. Les ULIS France         */
/*     Fax + 33 1 69 18 25 90                                                */
/*     e-mail: d.biringer@erd.advantest.de                                   */
/*                                                                           */
/* Originally written in C                                                   */
/*                                                                           */
/* Modification History:                                                     */
/*                                                                           */
/* May 1999 : New 'init' function. This driver can now be used to control    */
/*            the anlyzer via a RS-232 link except to read the trace         */
/*            New 'defaultInstrSetup' function : new function to return the  */
/*            scale, the freq offset status and the ref. offset status have  */
/*            been written                                                   */ 
/*            Modified by: Denis BIRINGER, Advantest Europe R&D.             */
/*===========================================================================*/
 
/*****************************************************************************/
/* INSTRUMENT-DEPENDENT COMMAND ARRAYS                                       */
/*****************************************************************************/
static ViString s_init[] = {"DL0","*IDN?",""}; 
static ViString s_freqType[] = {"CF","FA","FB"};         
static ViString s_spanType[] = {"SP","FS","ZS"};         
static ViString s_freqOffset[] = {"FOF","FON"};                
static ViString s_freqStepSize[] = {"CS","CA"};
static ViString s_counterResolution[] = {"CN0","CN1","CN2","CN3"};
static ViString s_counterONOFF[] = {"COUNT OFF","COUNT ON"};

static ViString s_refLevel[] = {"RL"};
static ViString s_refUnit[] = {"UB","UM","UU","UE","UW","","KSD","AUNITS W"};
static ViString s_yScale[] = {"DD10DB","DD5DB","DD2DB","DD1DB","DD0.5DB",
                              "LL1","LL2","LL5","LL10"};
static ViString s_refLevelOffset[] = {"ROF","RON"};
static ViString s_att[] = {"AT","AA"};
static ViString s_attMin[] = {"ATMIN ON","ATMIN OFF"};

static ViString s_RBW[] = {"BA","RB200HZ","RB300HZ","RB1KZ","RB3KZ","RB10KZ",
                           "RB30KZ","RB100KZ","RB300KZ","RB1MZ","RB3MZ","RB5MZ"};                    
static ViString s_VBW[] = {"VA","VB1HZ","VB3HZ","VB10HZ","VB30HZ","VB100HZ",
                           "VB300HZ","VB1KZ","VB3KZ","VB10KZ","VB30KZ","VB100KZ",
                           "VB300KZ","VB1MZ","VB3MZ","VB5MZ"};                
static ViString s_RBW_Span[] = {"CORS ON","CORS OFF"};
static ViString s_RBW_VBW[] = {"COVR ON","COVR OFF"};

static ViString s_All_Coupled[] = {"AL"};

static ViString s_sweepTime[] = {"SW","AS"};
static ViString s_sweepMode[] = {"SI","SN","SR","TS"}; 
static ViString s_windowSweep[] = {"WDOSWP OFF","WDOSWP ON"};
static ViString s_triggerSource[] = {"FR","LI","VI","EX"};
static ViString s_triggerLevel[] = {"TR"};
static ViString s_triggerSlope[] = {"TRIGSLP-","TRIGSLP+"};
static ViString s_gateSource[] = {"GTSRC IF","GTSRC GT","GTSRC EXT"};
static ViString s_gateSlope[] = {"GTSLP-","GTSLP+"};
static ViString s_gatedSweep[] = {"GTSWP OFF","GTSWP ON"};
static ViString s_gateFeatures[] = {"GTPOS","GTWID"};

static ViString s_traceAMode[] = {"AW","AV","AB","AM","AMIN","AG","CWA"};
static ViString s_averageMode[] = {"AG1","AG0"};  
static ViString s_traceBMode[] = {"BSTORE","BV","BB"};                            
static ViString s_tracePoints[] = {"TPL","TPS"};
static ViString s_detectorMode[] = {"DTN","DTP","DTG","DTS"};

static ViString s_showDL[] = {"DLF","DLN"};

static ViString s_LLType[] = {"LIMTYP TIME","LIMTYP FREQ"};
static ViString s_deleteLL[] = {"LMTBDEL","LMTADEL"};
static ViString s_setLL[] = {"LMTBIN","LMTAIN"};
static ViString s_showLL1[] = {"LAF","LAN"};
static ViString s_showLL2[] = {"LBF","LBN"}; 
static ViString s_levelX[] = {"LIMPOS ABS","LIMPOS REL"};
static ViString s_levelY[] = {"LIMAPOS ABS","LIMAPOS REL"};
static ViString s_shiftLL[] = {"LIMASFT","LIMSFT"};

static ViString s_positionWindow[]  = {"WLX","WDX"};
static ViString s_showWindow[]  = {"WDO OFF","WDO ON"};

static ViString s_showLC[]  = {"CR OFF","CR ON"};
static ViString s_workLC[]  = {"CRDEL","CRIN"};

static ViString s_mkrNormal[] = {"MKN"};
static ViString s_mkrDelta[] = {"MKD"};
static ViString s_fixedMkrDelta[] = {"FXF","FXN"};
static ViString s_1_deltaMkr[] = {"REDLT OFF","REDLT ON"};
static ViString s_displayMode[] = {"MDA","MDR"};
static ViString s_mkrTrace[] = {"MKTRACE TRB","MKTRACE TRA"};
static ViString s_mkrNDstatus[] = {"markers inactive","only marker normal active",
                                   "markers normal and delta active"};
static ViString s_multiMkrStatus[] = {"MLT"}; 
static ViString s_multiMkrON[] = {"MLN1","MLN2","MLN3","MLN4","MLN5","MLN6","MLN7",
                                  "MLN8","MLN9","MLN10"};
static ViString s_multiMkrOFF[] = {"MLF1","MLF2","MLF3","MLF4","MLF5","MLF6","MLF7",
                                   "MLF8","MLF9","MLF10"};
static ViString s_deleteMarker[] = {"MO"};                                  
                                  
static ViString s_continuousSearch[] = {"CPF","CPN"};
static ViString s_signalTrack[] = {"SGF","SGN"};
static ViString s_peakRange[] = {"PSN","PSU","PSL","DY"};
static ViString s_peakwindow[] = {"CPLMK OFF","CPLMK ON"};
static ViString s_peakSearch[] = {"MKPK","NXP","NXL","NXR","MIS","NXM"};
static ViString s_markerDeltaTo[] = {"MTCF","MTSP","MTCS","MTMKS"};
static ViString s_peakTo[] = {"MKRL","MKCF"};

static ViString s_traceMath[] = {"","ACHB","TR2","TR0","TR1"};
static ViString s_traceAverage[]  = {"AGR","AGS","AGP","AGC"};

static ViString s_readFreq[]= {"CF?","SP?","FA?","FB?"};
static ViString s_readFrequencyOffset[] = {"FO?"};
static ViString s_readFreqStepSize[] = {"CS?","CA?"};
static ViString s_readFreqCounter[] = {"COUNT?","MF?"};

static ViString s_readRefLevel[]={"RL?"};
static ViString s_readRefUnit[]={"AUNITS?"};
static ViString s_readUnitName[] = {"dBm","dBmV","dBµV","dBµVemf","dBpW","","V","W"};
static ViString s_readRefLevelOffset[]={"RO?"};

static ViString s_readSweepTime[] = {"SW?","AS?"};
static ViString s_readSweepMode[] = {"SWM?","Normal&full","Single&full",
                                     "Normal&window","Single&window"};
static ViString s_readTriggerMode[] = {"TM?","TR?","Free Run","Line","Video","External"};
static ViString s_readGatedSweep[] = {"GTSWP?","GTPOS?","GTWID?","GTSRC?"};
static ViString s_readGateSource[] = {"IF Signal","EXT gate in","EXT trigger"}; 

static ViString s_readRBWandVBW[] = {"VB?","RB?","VA?","BA?"};
static ViString s_readAtt[] = {"ATMIN?","AT?","","AA?"};
static ViString s_readScale[] = {"DD?","LN?","10dB/","5dB/","2dB/","1dB/","0.5dB/",
                                 "Lin x1","Lin x2", "Lin x5", "Lin x10"};
                                  
static ViString s_readMarkerTraceStatus[] = {"MKTRACE?","blank","Trace A","Trace B"};                                  
static ViString s_mkrNDsearch[] = {"MN?","MF?","ML?"};                                    
static ViString s_readMKDoptions[] = {"FX?"};                                   
static ViString s_multiMkrSearch[] = {"MLSF?","MLSL?"};
static ViString s_readmultiMkrStatus[] = {"MLT?"};
static ViString s_multiMkrMessage[] = {"multimarkers inactive","multimarkers active"};                                  
                                  
static ViString s_readPeakOptions[] = {"CP?","SG?","CPLMK?","DY?"};
static ViString s_readDisplayLine[] = {"DL?"};
static ViString s_readLLoptions[] = {"LIMAPOS?","LIMPOS?","LIMASFT?","LIMSFT?"};
static ViString s_readLLmessage[] = {"Absolute","Relative"};
static ViString s_LLjudgement[] = {"OPF?"};
static ViString s_LLjudgementText[] = {"PASS","UPPER FAIL","LOWER FAIL","UPPER&LOWER FAIL","ERROR"};
static ViString s_readMeasWindow[] = {"WLX?","WDX?","WDO?"};
static ViString s_readDetectorMode[] = {"DM?","Posi-Nega","Positive","Negative","Sample"};
static ViString s_readLLstatus[] = {"LMTB?","LMTA?"};
static ViString s_readTraceStatus[] = {"TB?","TA?"};
static ViString s_readTraceMessage[] = {"","B_View","B_Blank","A_Write","A_View",
                                        "A_Blank","A-DL->A","A-B->A","B-A->A",
                                        "A_Max","A_Avg","A_Min"};
static ViString s_readLevelCorrection[] = {"CR?"};                                        

static ViString s_textONOFF[] = {"inactive","active"};
static ViString s_readDisplayMode[] = {"absolute","relative"};
                                  
static ViString s_traceBinary[] = {"TBB?","TBA?"};
static ViString s_traceASCII[] = {"TAB?","TAA?"};  
                                  
static ViString s_calLevel[] = {"CLN"};                                  
static ViString s_calibration[]  = {"CLA","CLG","IT0","IT1","IT2","IT3","IT4","IT6"};
static ViString s_freqCorrection[] = {"FC OFF","FC ON"};
static ViString s_calCorrection[]  = {"CC OFF","CC ON"};

static ViString s_label[] = {"LOF","LON/"};
static ViString s_copyScreen[] = {"PRT GRY","PRT MOL","PRT MOS"};

static ViString s_clearStatusByte[] = {"S2"};

static ViString s_initCard[] = {"MMI /B:/","MMI /A:/"};
static ViString s_copyCard[] = {"ALLCOPY /A: B:/"};
static ViString s_selectCard[] = {"DEV /A:/","DEV /B:/","DEV /RAM:/"};
static ViString s_recallFile[] = {"RC/REG_","RC/"};
static ViString s_saveFile[] = {"SV/REG_","SV/"};
static ViString s_deleteFile[] = {"DEL/REG_","DEL/"};

static ViString s_reset[]={"*RST"};
static ViString s_errorQuery[]={"ERRNO?"};
static ViString s_instrRevision[] = {"REV?"};

static ViString s_timefreqUnit[] = {"SC","HZ"};

static ViString s_freqUnit[] = {"Hz","kHz","MHz","GHz"};
static ViString s_timeUnit[] = {"ns","us","ms","s"};

static ViString s_close[] = {"LCL"};

/*****************************************************************************/
/* INSTRUMENT-DEPENDENT STATUS/RANGE STRUCTURE                               */
/*****************************************************************************/

typedef struct  AVPA_stringValPair
{
    ViStatus stringVal;
    ViString stringName;
} AVPA_tStringValPair;
 

struct AVPA_instrFeatures {
    ViReal64 frequencyLow[3];
    ViReal64 frequencyHigh[3];
    ViReal64 spanLow;
    ViReal64 spanHigh;
    ViReal64 freqOffsetMin;
    ViReal64 freqOffsetMax;
    ViReal64 inputAttMin;
    ViReal64 inputAttMax;
    ViReal64 refLevelMin[8];
    ViReal64 refLevelMax[8];
    ViReal64 refLevelOffsetMin;
    ViReal64 refLevelOffsetMax;
    ViReal64 ratio_RBW_SpanMin;
    ViReal64 ratio_RBW_SpanMax;
    ViReal64 ratio_RBW_VBWMin;
    ViReal64 ratio_RBW_VBWMax;
    ViReal64 sweepTimeMini;
    ViReal64 sweepTimeMaxi;
    ViReal64 gatePositionMini;
    ViReal64 gatePositionMaxi;
    ViReal64 gateWidthMini;
    ViReal64 gateWidthMaxi;
    ViInt16  averageCountMini;
    ViInt16  averageCountMaxi;
    ViInt16  DL_LevelMin;
    ViInt16  DL_LevelMax;
    ViInt16  displayMode;
    ViInt16  scale;
    ViInt16  minAttControl;
    ViInt16  tracePoints;
    ViInt16  DLstatus;
    ViInt16  refLevelOffsetStatus;
    ViInt16  freqOffsetStatus;
    ViUInt32 instrumentType;
    ViReal64 deltaYmin;
    ViReal64 deltaYmax;
    ViReal64 calLevelMin;
    ViReal64 calLevelMax;
    ViInt16 MKRMstatus[10]; /*0 if multi-marker inactive, 1 if muti-marker active*/
    ViChar   instrDriverRevision[256];
};  
    
typedef struct AVPA_instrFeatures *AVPA_ptrFeatures;

/*****************************************************************************/
/* UTILITY ROUTINE DECLARATIONS (Non-Exportable Functions)                   */
/*****************************************************************************/
ViBoolean AVPA_invalidViBooleanRange (ViBoolean val);
ViBoolean AVPA_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max);
ViBoolean AVPA_invalidViInt32Range (ViInt32 val, ViInt32 min, ViInt32 max);
ViBoolean AVPA_invalidViUInt16Range (ViUInt16 val, ViUInt16 min, ViUInt16 max);
ViBoolean AVPA_invalidViUInt32Range (ViUInt32 val, ViUInt32 min, ViUInt32 max);
ViBoolean AVPA_invalidViReal32Range (ViReal32 val, ViReal32 min, ViReal32 max);
ViBoolean AVPA_invalidViReal64Range (ViReal64 val, ViReal64 min, ViReal64 max);
ViStatus AVPA_initCleanUp (ViSession openRMSession, ViPSession openInstrSession,
                              ViStatus currentStatus);
ViStatus AVPA_defaultInstrSetup (ViSession instrSession, ViRsrc resourceName, ViUInt32 instrType);
ViStatus AVPA_initReset (ViSession, ViRsrc, ViUInt32);
ViStatus AVPA_Search_Scale (ViSession, ViPInt16);
ViStatus AVPA_Search_RefOffsetStatus (ViSession, ViPInt16);
ViStatus AVPA_Search_FreqOffsetStatus (ViSession, ViPInt16);

/*****************************************************************************/
/* USER-CALLABLE FUNCTIONS (Exportable Functions)                            */
/*****************************************************************************/

/*===========================================================================*/
/* Function: Initialize                                                      */
/* Purpose:  This function opens the instrument, queries the instrument      */
/*           for its ID, and initializes the instrument to a known state.    */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_init (ViRsrc resourceName, ViBoolean IDQuery, ViBoolean reset, ViPSession instrSession)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViSession rmSession = 0;
    ViUInt32 retCnt = 0, AVPA_instrType;
    ViByte Buffer[BUFFER_SIZE];
    ViByte cable; 
    
    /*- Check input parameter ranges ----------------------------------------*/
    if (AVPA_invalidViBooleanRange (IDQuery))
        return VI_ERROR_PARAMETER2;
    if (AVPA_invalidViBooleanRange (reset))
        return VI_ERROR_PARAMETER3;

    /*- Open instrument session ---------------------------------------------*/
    
    if ((AVPA_status = viOpenDefaultRM (&rmSession)) < 0)
        return AVPA_status;

    if (( AVPA_status = viOpen(rmSession, resourceName, VI_NULL, VI_NULL, instrSession)) < 0) {
        viClose (rmSession);
        return AVPA_status;
    }

    /*- Configure VISA Formatted I/O ----------------------------------------*/
    
    cable = resourceName[0]; /* GPIB or RS-232 cable */
    
    if (cable=='G') { /*GPIB cable*/

        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 10000)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
        if ((AVPA_status = viSetBuf (*instrSession, VI_READ_BUF|VI_WRITE_BUF, 4000)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_WR_BUF_OPER_MODE,
                                           VI_FLUSH_ON_ACCESS))   < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_RD_BUF_OPER_MODE,
                                           VI_FLUSH_ON_ACCESS))   < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
        
        Fmt (Buffer,"%s<%s",s_init[0]);     
        if ((AVPA_status = viWrite(*instrSession,Buffer,NumFmtdBytes(),&retCnt)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
    }
    
    else { /*RS-232 cable*/
        /* Serial Interface initialized to 8 databits, 1 stopbit, no parity, */
        /* baud rate = 19200, flow control = XonXoff and timeout = 20000 ms  */
        
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_DATA_BITS, 8))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
    
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_STOP_BITS,VI_ASRL_STOP_ONE))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
        
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_BAUD, 19200))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
        
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_FLOW_CNTRL,VI_ASRL_FLOW_XON_XOFF))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
        
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_TMO_VALUE, 20000)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
   
        if ((AVPA_status = viSetBuf (*instrSession, VI_ASRL_IN_BUF|VI_ASRL_OUT_BUF, 512)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
   
        if ((AVPA_status = viSetBuf (*instrSession, VI_READ_BUF|VI_WRITE_BUF, 4000)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
   
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_WR_BUF_OPER_MODE,VI_FLUSH_ON_ACCESS))<0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
    
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_RD_BUF_OPER_MODE,VI_FLUSH_ON_ACCESS)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
 
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_TERMCHAR, '\n')) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
 
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_END_IN,VI_ASRL_END_TERMCHAR)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
   
        if ((AVPA_status = viSetAttribute (*instrSession, VI_ATTR_ASRL_END_OUT,VI_ASRL_END_NONE)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);       

        // sends an empty string to the analyzer to init the RS-232 link //
        Fmt (Buffer,"%s<%s",s_init[2]);    
        if ((AVPA_status = viWrite(*instrSession,Buffer,NumFmtdBytes(),&retCnt)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
            
    }               
            
    /*- Identification Query ------------------------------------------------*/
    
    if (IDQuery) {
        
        Fmt (Buffer,"%s<%s",s_init[1]);     
        if ((AVPA_status = viWrite(*instrSession,Buffer,NumFmtdBytes(),&retCnt)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
    
        if ((AVPA_status = viRead (*instrSession, Buffer, BUFFER_SIZE, &retCnt)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
        
        Scan (Buffer, "ADVANTEST,R3263");
        if (NumFmtdBytes () != 15) {
            Scan (Buffer, "ADVANTEST,R3272");
            if (NumFmtdBytes () != 15) { 
                Scan (Buffer, "ADVANTEST,R3463");
                if (NumFmtdBytes () != 15) { 
                    Scan (Buffer, "ADVANTEST,R3465");
                    if (NumFmtdBytes () != 15) 
                        return AVPA_initCleanUp (rmSession, instrSession, VI_ERROR_FAIL_ID_QUERY);
                }
            }
        }
        if (Scan(Buffer, "%s[i11]>%d[b4]", &AVPA_instrType)!=1) {
            AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
            return AVPA_initCleanUp (rmSession, instrSession, VI_ERROR_INSTR_INTERPRETING_RESPONSE);
        } 
    }
    else AVPA_instrType = 3263; //Default InstrType if IDQuery not done
   
    /*- Reset instrument ----------------------------------------------------*/
    if (reset) {
        if ((AVPA_status = AVPA_initReset (*instrSession, resourceName, AVPA_instrType)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
    }       
    else { /*- Send Default Instrument Setup ---------------------------------*/ 
        if ((AVPA_status = AVPA_defaultInstrSetup (*instrSession, resourceName, AVPA_instrType)) < 0)
            return AVPA_initCleanUp (rmSession, instrSession, AVPA_status);
    }
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function : configure and read                                             */
/* Purpose : application function                                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_configRead(ViSession instrID,ViBoolean trace,ViInt16 peakSearch,
                                  ViInt16 _VI_FAR traceArray[],ViPReal64 sweepTime,
                                  ViChar _VI_FAR sweepUnit[],ViPReal64 MKRNposition,
                                  ViPReal64 MKRNlevel)
                                  
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViInt16 control;
    ViReal64 sweepValue;
    
    if (AVPA_invalidViBooleanRange (trace))             
        return VI_ERROR_PARAMETER2;
        
    if (AVPA_invalidViInt16Range (peakSearch,0,5))             
        return VI_ERROR_PARAMETER3;
       
    if ((AVPA_status = AVPA_Read_TraceData (instrID, trace,0,traceArray))<0)
        return AVPA_status;
    
    if ((AVPA_status = AVPA_Search_Peak (instrID, peakSearch)) <0)
        return AVPA_status;   
       
    if ((AVPA_status = AVPA_Read_SweepTime (instrID, &sweepValue, &control))<0)
        return AVPA_status;
       
    if ((AVPA_status = AVPA_Convert_Time (instrID, sweepValue, sweepTime, sweepUnit))<0)
        return AVPA_status;
       
    if ((AVPA_status = AVPA_Read_MarkerNormal (instrID, MKRNposition, MKRNlevel))<0)
        return AVPA_status;   
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Frequency                                                   */           
/* Purpose:  sets up center, start or stop frequency.                        */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_Freq (ViSession instrID, ViInt16 freqType,
                                 ViReal64 freqValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
        
    if (AVPA_invalidViInt16Range (freqType, 0, 2))             
        return VI_ERROR_PARAMETER2;
        
    if (AVPA_invalidViReal64Range (freqValue, featuresPtr->frequencyLow[freqType],
                                   featuresPtr->frequencyHigh[freqType]))             
        return VI_ERROR_PARAMETER3;
    
    Fmt (wrtBuffer,"%s<%s %fHZ",s_freqType[freqType],freqValue);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
                                  
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set  Frequency Span                                             */           
/* Purpose:  sets up the frequency span.                                     */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_Span (ViSession instrID, ViInt16 spanType,
                                 ViReal64 spanValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViInt16Range (spanType, 0, 2))             
        return VI_ERROR_PARAMETER2;                         
       
    if (spanType==0) {  /* Manual Span Setting */
    	if (AVPA_invalidViReal64Range (spanValue, featuresPtr->spanLow, 
                                                  featuresPtr->spanHigh))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s %fHZ",s_spanType[spanType],spanValue);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else { /*full span or zero span*/
        Fmt (wrtBuffer,"%s<%s",s_spanType[spanType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    return AVPA_status;
}                         

/*===========================================================================*/
/* Function: Set Frequency Offset                                            */           
/* Purpose:  sets up frequency offset and activates frequency offset         */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_FreqOffset (ViSession instrID, ViBoolean activation,
                                       					  ViReal64 freqOffset)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    if (AVPA_invalidViBooleanRange (activation))             
        return VI_ERROR_PARAMETER2;   

    if (activation == VI_ON) {
    	if (AVPA_invalidViReal64Range (freqOffset,featuresPtr->freqOffsetMin,
                                              	  featuresPtr->freqOffsetMax))             
        	return VI_ERROR_PARAMETER3;
        featuresPtr->freqOffsetStatus = 1;
        Fmt (wrtBuffer,"%s<%s%fHZ", s_freqOffset[activation], freqOffset);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else {
        featuresPtr->freqOffsetStatus = 0;  
        Fmt (wrtBuffer,"%s<%s",s_freqOffset[activation]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Frequency Step Size                                         */           
/* Purpose:  sets up and activates or desactivates Frequency Step Size       */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_FreqStepSize (ViSession instrID, ViBoolean controlType,
                                         ViReal64 freqStepSize)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;
       
    if (controlType == VI_ON) { /*step size chosen by the analyzer*/ 
        Fmt (wrtBuffer,"%s<%s", s_freqStepSize[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else { /*step size chosen by the user*/   
    	if (AVPA_invalidViReal64Range (freqStepSize,featuresPtr->spanLow,
                                                	featuresPtr->spanHigh))             
        	return VI_ERROR_PARAMETER3; 
        Fmt (wrtBuffer,"%s<%s%fHZ",s_freqStepSize[controlType],freqStepSize);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Frequency Counter                                           */
/* Purpose:  Sets the counter ON / OFF or sets the counter resolution        */
/*===========================================================================*/                               
ViStatus _VI_FUNC AVPA_Set_FreqCounter (ViSession instrID, ViBoolean activation,
                                        ViInt16 resolution)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (activation))             
        return VI_ERROR_PARAMETER2;
       
    if (AVPA_invalidViInt16Range (resolution,0,3))             
        return VI_ERROR_PARAMETER3;
       
    if (activation == VI_ON) {
        Fmt (wrtBuffer,"%s<%s", s_counterResolution[resolution]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status; 
    }

    Fmt (wrtBuffer,"%s<%s", s_counterONOFF[activation]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;

    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Reference Level                                             */           
/* Purpose:  sets up the reference level (Y axis).                           */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RefLevel (ViSession instrID, ViReal64 refLevel)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViInt16 unit=0;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    /* search of the reference unit because the range of validity for the 
       reference level depends on the reference unit*/
    
    Fmt (Buffer,"%s<%s",s_readRefUnit[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 3, &retCount)) < 0)
        return AVPA_status;
    
    if (Scan(Buffer,"%s>%d[b2]", &unit)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
       
    /* sets up the reference level */
    
    if (AVPA_invalidViReal64Range (refLevel,featuresPtr->refLevelMin[unit],
                                            featuresPtr->refLevelMax[unit]))             
        return VI_ERROR_PARAMETER2;
       
    Fmt (Buffer,"%s<%s%f",s_refLevel[0],refLevel);     
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Reference Unit                                              */           
/* Purpose:  Set up the unit of the reference level.                         */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RefUnit (ViSession instrID, ViInt16 refUnit)
{   
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (refUnit,0,7))             
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuffer,"%s<%s",s_refUnit[refUnit]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Scale                                                       */           
/* Purpose:  Set up the scale of the Y axis .                                */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_Scale (ViSession instrID, ViInt16 scale)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViInt16Range (scale,0,8))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_yScale[scale]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    featuresPtr->scale = scale;   
                                  
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Reference Level Offset                                      */           
/* Purpose:  Set up and activates the reference level offset.                */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RefOffset ( ViSession instrID, ViBoolean activation,
                                                          ViReal64 refLevelOffset)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    if (AVPA_invalidViBooleanRange (activation))             
        return VI_ERROR_PARAMETER2;   

    if (activation == VI_ON) {
    	if (AVPA_invalidViReal64Range (refLevelOffset,featuresPtr->refLevelOffsetMin,
                                                  	  featuresPtr->refLevelOffsetMax))             
        	return VI_ERROR_PARAMETER3;  
        Fmt (wrtBuffer,"%s<%s%fDB", s_refLevelOffset[activation], refLevelOffset);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else {
        Fmt (wrtBuffer,"%s<%s",s_refLevelOffset[activation]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    
    featuresPtr->refLevelOffsetStatus = activation; 
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Input Attenuation                                           */           
/* Purpose:  Set up the Input attenuation.                                   */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_Att (ViSession instrID, ViBoolean controlType,
                                                   ViReal64 inputAttValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;   

    if (controlType == VI_ON) { /* attenuation chosen by the analyzer */ 
        Fmt (wrtBuffer,"%s<%s", s_att[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
             return AVPA_status;
    }
    else { /* attenuation chosen by the user */
    	if (AVPA_invalidViReal64Range (inputAttValue, featuresPtr->inputAttMin,
                                                  	  featuresPtr->inputAttMax))             
        	return VI_ERROR_PARAMETER3; 
        Fmt (wrtBuffer,"%s<%s%f",s_att[controlType],inputAttValue);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Minimum Input Attenuation                                   */           
/* Purpose:  Set up the minimum attenuation that can be set                  */           
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_MinATT (ViSession instrID, ViBoolean controlType,
                                                      ViReal64 minInputAttValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;   

    if (controlType == VI_ON) { /* minimum attenuation chosen by the analyzer */
        Fmt (wrtBuffer,"%s<%s", s_attMin[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        featuresPtr->minAttControl = 1;    
    }
    else { /* minimum attenuation chosen by the user */   
    	if (AVPA_invalidViReal64Range (minInputAttValue, featuresPtr->inputAttMin,
                                                     	 featuresPtr->inputAttMax))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s%f",s_attMin[controlType],minInputAttValue);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        featuresPtr->minAttControl = 0;   
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set RBW                                                         */           
/* Purpose:  Set up the Resolution Bandwidth.                                */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RBW (ViSession instrID, ViInt16 RBWsetting)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViInt16Range (RBWsetting,0,11))             
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuffer,"%s<%s",s_RBW[RBWsetting]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set VBW                                                         */           
/* Purpose:  Set up the Video Bandwidth.                                     */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_VBW (ViSession instrID, ViInt16 VBWsetting)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViInt16Range (VBWsetting,0,15))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_VBW[VBWsetting]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;    

    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set RBW/Span Coupling                                           */           
/* Purpose:  Sets the ratio that defines the automatic coupling between      */
/*           RBW and the Span                                                */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RBW_Span (ViSession instrID, ViBoolean controlType,
                                                        ViReal64 ratio_RBW_Span)
{
    ViStatus AVPA_status = VI_SUCCESS; 
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;
        
    if (controlType == VI_ON) { /*ratio chosen by the analyzer*/
        Fmt (wrtBuffer,"%s<%s",s_RBW_Span[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    else  { /*ratio chosen by the the user*/
    	if (AVPA_invalidViReal64Range (ratio_RBW_Span,featuresPtr->ratio_RBW_SpanMin,
                                                  	  featuresPtr->ratio_RBW_SpanMax))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s%f",s_RBW_Span[controlType],ratio_RBW_Span);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set RBW/VBW Coupling                                            */           
/* Purpose:  Sets the ratio that defines the automatic coupling between      */
/*           VBW and RBW                                                     */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_RBW_VBW (ViSession instrID, ViBoolean controlType,
                                                       ViReal64 ratio_RBW_VBW)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;
        
    if (controlType == VI_ON) { /*ratio chosen by the analyzer*/
        Fmt (wrtBuffer,"%s<%s",s_RBW_VBW[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    else { /*ratio chosen by the user*/
    	if (AVPA_invalidViReal64Range (ratio_RBW_VBW,featuresPtr->ratio_RBW_VBWMin,
                                   	                 featuresPtr->ratio_RBW_VBWMax))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s%f",s_RBW_VBW[controlType],ratio_RBW_VBW);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set All Auto Couple                                             */           
/* Purpose:  Sets all couple functions to automatic mode                     */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_All_Auto_Couple (ViSession instrID)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    Fmt (wrtBuffer,"%s<%s",s_All_Coupled[0]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status; 
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Set sweep time                                                  */           
/* Purpose:  Sets the sweep time (sec)                                       */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_SweepTime (ViSession instrID, ViBoolean controlType,
                                      ViReal64 sweepTime)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    if (AVPA_invalidViBooleanRange (controlType))             
        return VI_ERROR_PARAMETER2;
        
    if (controlType == VI_ON) { /*sweep time is chosen by the analyzer*/  
        Fmt (wrtBuffer,"%s<%s",s_sweepTime[controlType]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    else { /*sweep time is chosen by the user*/ 
    	if (AVPA_invalidViReal64Range (sweepTime,featuresPtr->sweepTimeMini,
                                             	 featuresPtr->sweepTimeMaxi))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s%f",s_sweepTime[controlType],sweepTime);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;  
    }
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set sweep mode                                                  */           
/* Purpose:  Sets the sweep mode: continuous / single, window sweep on / off */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_SweepMode (ViSession instrID, ViBoolean sweepMode,
                                      ViBoolean windowSweep)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViBooleanRange (sweepMode))             
        return VI_ERROR_PARAMETER2;
       
    if (AVPA_invalidViBooleanRange (windowSweep))             
        return VI_ERROR_PARAMETER3;
     
    Fmt (wrtBuffer,"%s<%s,%s",s_sweepMode[sweepMode],s_windowSweep[windowSweep]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set trigger mode                                                */           
/* Purpose:  Defines the trigger source,the slope of the external trigger    */
/*           and the level of the video trigger                              */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_TriggerMode(ViSession instrID, ViInt16 triggerSource,
                                       ViBoolean triggerPolarity,ViInt16 triggerLevel)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViInt16Range (triggerSource,0,3))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_triggerSource[triggerSource]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status; 

    if (triggerSource == 2) { /*video trigger*/
    	if (AVPA_invalidViInt16Range (triggerLevel,0,100))             
        	return VI_ERROR_PARAMETER4;
        Fmt (wrtBuffer,"%s<%s%d[b2]",s_triggerLevel[0],triggerLevel);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    if (triggerSource == 3) {  /*external trigger*/
    	if (AVPA_invalidViBooleanRange (triggerPolarity))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s",s_triggerSlope[triggerPolarity]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set gated sweep                                                 */           
/* Purpose:  Configures gated sweep mode: gate position and width are also   */
/*           defined here.                                                   */
/*===========================================================================*/ 
ViStatus _VI_FUNC AVPA_Set_GatedSweep (ViSession instrID,ViBoolean gatedSweep,
                                       ViInt16 gateSource, ViBoolean gateSlope,
                                       ViReal64 gatePosition,ViReal64 gateWidth)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (gatedSweep))             
        return VI_ERROR_PARAMETER2;
    
    if (gatedSweep == VI_ON) {
    
    	if (AVPA_invalidViInt16Range (gateSource,0,2))             
        	return VI_ERROR_PARAMETER3;
        	
    	if (AVPA_invalidViReal64Range (gatePosition,featuresPtr->gatePositionMini,
        	                                        featuresPtr->gatePositionMaxi))             
        	return VI_ERROR_PARAMETER5;
        
    	if (AVPA_invalidViReal64Range (gateWidth,featuresPtr->gateWidthMini,
        	                                     featuresPtr->gateWidthMaxi))             
        	return VI_ERROR_PARAMETER6;
      
        /* sets up the gate source*/
        Fmt (wrtBuffer,"%s<%s",s_gateSource[gateSource]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        if (gateSource == 2) { /*external trigger*/
            if (AVPA_invalidViBooleanRange (gateSlope))             
        		return VI_ERROR_PARAMETER4;
            Fmt (wrtBuffer,"%s<%s",s_gateSlope[gateSlope]);     
            if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
                return AVPA_status;
        }
         
        /* Set up the gate */
        Fmt (wrtBuffer,"%s<%s%f,%s%f",s_gateFeatures[0],gatePosition
                                     ,s_gateFeatures[1],gateWidth);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
        
    }
     
    Fmt (wrtBuffer,"%s<%s",s_gatedSweep[gatedSweep]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
            
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Trace A Mode                                                */           
/* Purpose:  Defines Trace A mode (Write, View, Max Hold ...)                */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_TraceAMode (ViSession instrID, ViInt16 traceAMode,
                                       ViInt16 averageCount, ViBoolean averageMode)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViInt16Range (traceAMode,0,6))             
        return VI_ERROR_PARAMETER2;
    
    if (traceAMode != 5) { /* set trace mode except trace average */ 
        Fmt (wrtBuffer,"%s<%s",s_traceAMode[traceAMode]);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
    else { /* set trace average: some parameters are needed */
    	if (AVPA_invalidViInt16Range (averageCount,featuresPtr->averageCountMini,
                                               	   featuresPtr->averageCountMaxi))             
        	return VI_ERROR_PARAMETER3;
    	if (AVPA_invalidViBooleanRange (averageMode))             
        	return VI_ERROR_PARAMETER4;
        Fmt (wrtBuffer,"%s<%s,%s%d[b2]",s_averageMode[averageMode],
                                       s_traceAMode[traceAMode],averageCount);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Trace B Mode                                                */           
/* Purpose:  Defines Trace B mode : Store, View or Blank                     */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_TraceBMode (ViSession instrID, ViInt16 traceBMode)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (traceBMode,0,2))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_traceBMode[traceBMode]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Set Detector Mode                                               */           
/* Purpose:  Specifies detection mode: posi-nega, posi-peak...               */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_TraceDetector (ViSession instrID, ViInt16 detectorMode) 
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViInt16Range (detectorMode,0,3))             
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuffer,"%s<%s",s_detectorMode[detectorMode]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Set Trace Points                                                */           
/* Purpose:  Specifies the number of points in the traces                    */
/*===========================================================================*/ 
ViStatus _VI_FUNC AVPA_Set_TracePoints (ViSession instrID, ViBoolean tracePoints)
{
    ViStatus AVPA_status = VI_SUCCESS;  
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViBooleanRange (tracePoints))             
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuffer,"%s<%s",s_tracePoints[tracePoints]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if (tracePoints == VI_ON) featuresPtr->tracePoints = 501;
    else featuresPtr->tracePoints = 1001;
    
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Set Limit Line                                                  */
/* Purpose:  Defines or clears a whole limit line table                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_LimitLine (ViSession instrID, ViBoolean limitLine,
                                      ViString LL_FreqTimeTable,
                                      ViString LL_LevelTable)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViReal64 freqtime[27];
    ViReal64 level[27];
    ViReal64 span;
    ViInt16 i=0;
    
    if (AVPA_invalidViBooleanRange (limitLine))             
        return VI_ERROR_PARAMETER2;
    
    for (i=0; i<27; i++) {
        freqtime[i]=0;
        level[i]=0;
    }
    
    Scan(LL_FreqTimeTable,"%s>%27f[x]",freqtime);
    Scan(LL_LevelTable,"%s>%27f[x]",level);
    
    /* which is the work domain : frequency or time domain ? */
    
    Fmt (Buffer,"%s<%s",s_readFreq[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", &span)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    if (span !=0 ) span = 1; else span = 0;
     
    Fmt (Buffer,"%s<%s,%s",s_LLType[(ViInt16)span],s_deleteLL[limitLine]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    i=0;
    while ( (i<=24)&&((freqtime[i]!=0)||(freqtime[i+1]!=0)||(freqtime[i+2]!=0)) ) {
        Fmt (Buffer,"%s<%s%f%s,%fDB",s_setLL[limitLine],freqtime[i],
                                    s_timefreqUnit[(ViInt16)span],level[i]);   
        if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
        i++;   
    }   
     
    return AVPA_status; 
}                        

/*===========================================================================*/
/* Function: Activate Limit Line                                             */
/* Purpose:  Show Limit Line 1 and/or Limit Line 2                           */
/*           Choose between absolute or relative along the two axis          */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Activate_LimitLine (ViSession instrID,
                                           ViBoolean LL1Status,ViBoolean LL2Status,
                                           ViBoolean levelXaxis, ViBoolean levelYaxis)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViBooleanRange (LL1Status))             
        return VI_ERROR_PARAMETER2;
        
    if (AVPA_invalidViBooleanRange (LL2Status))             
        return VI_ERROR_PARAMETER3;
        
    if (AVPA_invalidViBooleanRange (levelXaxis))             
        return VI_ERROR_PARAMETER4;
    
    if (AVPA_invalidViBooleanRange (levelYaxis))             
        return VI_ERROR_PARAMETER5;    
    
    Fmt (wrtBuffer,"%s<%s,%s,%s,%s",s_showLL1[LL1Status],s_showLL2[LL2Status],
                                    s_levelX[levelXaxis],s_levelY[levelYaxis]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Move Limit Line                                                 */           
/* Purpose:  Enables to shift both Limit Lines                               */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Move_LimitLine (ViSession instrID, ViBoolean shiftType,
                                       ViReal64 shiftValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViReal64 span;

    if (AVPA_invalidViBooleanRange (shiftType))             
        return VI_ERROR_PARAMETER2;
        
    /* which is the work domain : frequency or time domain ? */
    
    Fmt (Buffer,"%s<%s",s_readFreq[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", &span)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }     
    
    if (span != 0)  span=1; else span=0;
     
    if (shiftType == VI_ON) { /*position's shift*/
        Fmt (Buffer,"%s<%s,%s%f%s",s_LLType[(ViInt16)span],s_shiftLL[shiftType],
                                  shiftValue,s_timefreqUnit[(ViInt16)span]);   
        if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
    else {  /*level's shift*/ 
        Fmt (Buffer,"%s<%s,%s%fDB",s_LLType[(ViInt16)span],s_shiftLL[shiftType],
                                                                    shiftValue);   
        if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
     
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Set Display Line                                                */           
/* Purpose:  Specifies display line position and show or hide it             */
/*===========================================================================*/           
ViStatus _VI_FUNC AVPA_Set_DisplayLine (ViSession instrID,ViBoolean showDL,
                                        ViReal64 DL_Level)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (showDL))             
        return VI_ERROR_PARAMETER2;
    
    if (showDL == VI_ON) {
    	if (AVPA_invalidViReal64Range (DL_Level,featuresPtr->DL_LevelMin,
            	                                featuresPtr->DL_LevelMax))             
        	return VI_ERROR_PARAMETER3;
        Fmt (wrtBuffer,"%s<%s%f",s_showDL[showDL],DL_Level);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
    else {
        Fmt (wrtBuffer,"%s<%s",s_showDL[showDL]);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    } 
     
    featuresPtr->DLstatus = showDL; 
      
    return AVPA_status;                                          
}                        

/*===========================================================================*/
/* Function: Set measurement Window                                          */
/* Purpose:  Sets center position, Window width and shows or hides           */
/*           mesurement window                                               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MeasurementWindow (ViSession instrID,
                                              ViBoolean windowStatus,
                                              ViReal64 windowPosition,
                                              ViReal64 windowWidth)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (windowStatus))             
        return VI_ERROR_PARAMETER2;
        
    if (windowStatus == VI_OFF) {
        Fmt (wrtBuffer,"%s<%s",s_showWindow[windowStatus]);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    }
    else {
        Fmt (wrtBuffer,"%s<%s%f,%s%f,%s",s_positionWindow[0],windowPosition,
                                       s_positionWindow[1],windowWidth,
                                       s_showWindow[windowStatus]);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    } 
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: set level correction                                            */
/* Purpose:  Activates and sets or desactivates level correction function    */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_LevelCorrection (ViSession instrID, ViBoolean LCstatus,
                                            ViString LC_FreqTable,
                                            ViString LC_LevelTable)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViReal64 freq[27];
    ViReal64 level[27];
    ViInt16 i=0;
    
    if (AVPA_invalidViBooleanRange (LCstatus))             
        return VI_ERROR_PARAMETER2;
        
    if (LCstatus == VI_ON) {
      
        for (i=0; i<27; i++) {
            freq[i]=0;
            level[i]=0;
        }
    
        Scan(LC_FreqTable,"%s>%27f[x]",freq);
        Scan(LC_LevelTable,"%s>%27f[x]",level);
    
        Fmt (wrtBuffer,"%s<%s",s_workLC[0]);   
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
         
        i=0;
        while ( (i<=24)&&((freq[i]!=0)||(freq[i+1]!=0)||(freq[i+2]!=0)) ) {
            Fmt (wrtBuffer,"%s<%s%fHZ,%fDB",s_workLC[1],freq[i],level[i]);   
            if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
                return AVPA_status;
            i++;   
        }
    }   
       
    Fmt (wrtBuffer,"%s<%s",s_showLC[LCstatus]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status; 
     
    return AVPA_status; 
}                        

/*===========================================================================*/
/* Function: Set Marker Normal                                               */
/* Purpose:  sets the marker normal (frequency or time)                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MarkerNormal (ViSession instrID, ViReal64 position)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (wrtBuffer,"%s<%s%f",s_mkrNormal[0],position);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
    	return AVPA_status;   
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set Marker Delta                                                */
/* Purpose:  Sets the marker delta position                                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MarkerDelta (ViSession instrID, ViReal64 position)
{
    ViStatus AVPA_status = VI_SUCCESS; 
    ViByte wrtBuffer[BUFFER_SIZE];      
    ViUInt32 retCount=0;
    
    Fmt (wrtBuffer,"%s<%s%f",s_mkrDelta[0],position);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
    	return AVPA_status;
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Set marker delta options                                        */
/* Purpose:  Sets the display mode and activates or desactivates marker      */
/*           delta fixed and 1/delta marker                                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MarkerDeltaOptions (ViSession instrID,
                                               ViBoolean divided,
                                               ViBoolean fixed,
                                               ViBoolean displayMode)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];      
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViBooleanRange (divided))             
        return VI_ERROR_PARAMETER2;
       
    if (AVPA_invalidViBooleanRange (fixed))             
        return VI_ERROR_PARAMETER3;
       
    if (AVPA_invalidViBooleanRange (displayMode))             
        return VI_ERROR_PARAMETER4;

    Fmt (wrtBuffer,"%s<%s,%s,%s",s_1_deltaMkr[divided],s_fixedMkrDelta[fixed],
                                 s_displayMode[displayMode]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
          
    featuresPtr->displayMode = displayMode; 
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function : Set Marker Trace                                               */
/* Purpose : chooses the trace on which the markers will be active           */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MarkerTrace (ViSession instrID, ViBoolean markerTrace)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (markerTrace))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (Buffer,"%s<%s",s_mkrTrace[markerTrace]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    return AVPA_status;
}                            

/*===========================================================================*/
/* Function: Activate Multimarkers                                           */
/* Purpose:  activates the multimarkers                                      */
/*===========================================================================*/                                  
ViStatus _VI_FUNC AVPA_Activate_Multimarkers (ViSession instrID)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_multiMkrStatus[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
    	return AVPA_status;
    
    return AVPA_status;                              
}                         

/*===========================================================================*/
/* Function: Set Multimarkers                                                */
/* Purpose:  sets the Multi_Markers (frequency or time)                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_Multimarkers (ViSession instrID, ViString multiMkrStatus,
                                                            ViString multiMkrPositions)
                                                            
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViReal64 freqtime[10];
    ViInt16 i=0;
    AVPA_ptrFeatures featuresPtr;
    

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    for (i=0; i<10; i++) {
        freqtime[i]=0;
    }
    
    Scan(multiMkrStatus,"%s>%10d[b2x]",featuresPtr->MKRMstatus);
    Scan(multiMkrPositions,"%s>%10f[x]",freqtime);
    
    for (i=0; i<10; i++) {
        if (featuresPtr->MKRMstatus[i]!= 0) {
            Fmt (wrtBuffer,"%s<%s%f",s_multiMkrON[i],freqtime[i]);   
            if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
                return AVPA_status;
        }
        else {
            Fmt (wrtBuffer,"%s<%s",s_multiMkrOFF[i]);   
            if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
                return AVPA_status;
        }
    }
    
    return AVPA_status; 
}                                                                                   

/*===========================================================================*/
/* Function: Set ActiveMultimarker                                           */
/* Purpose:  chooses the active Multi-Marker                                 */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_ActiveMultiMarker (ViSession instrID, ViInt16 index)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViInt16 status=0, delta=0;
    ViReal64 multiMkrPositions[10];
    ViReal64 mkrDeltaPosition;
    ViReal64 multiMkrLevels[10];
    ViReal64 mkrDeltaLevel;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViInt16Range(index,1,10))
    	return VI_ERROR_PARAMETER2;
    	
    /* returns whether the delta marker is active or not */
    Fmt (Buffer,"%s<%s",s_mkrNDsearch[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", &status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    if (status==2) {
    	delta = 1;
    }
    else {
    	delta = 0;
    }	
    
    /* reads the positions of the multimarkers*/
    AVPA_Read_Multimarkers (instrID, multiMkrPositions,&mkrDeltaPosition,
                                     multiMkrLevels,&mkrDeltaLevel);
    
    if (featuresPtr->MKRMstatus[index-1]!=0) {
    	Fmt (Buffer,"%s<%s%f",s_multiMkrON[index-1],multiMkrPositions[index-1]+delta*mkrDeltaPosition);   
    	if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        	return AVPA_status;
    }
    else {
    	return VI_ERROR_MARKERINACTIVE;
    }	
    	
    return AVPA_status; 
}  

/*===========================================================================*/
/* Function: Delete Markers                                                  */
/* Purpose:  deletes all the markers active (normal, delta, multimarkers)    */
/*===========================================================================*/                                  
ViStatus _VI_FUNC AVPA_Delete_Markers (ViSession instrID)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s\r\n",s_deleteMarker[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    return AVPA_status;                              
} 

/*===========================================================================*/
/* Function: Set Peak Search Options                                         */
/* Purpose:  Sets the features of the peak search                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_PeakSearchOptions (ViSession instrID,
                                              ViBoolean continuousSearch,
                                              ViBoolean signalTrack,
                                              ViBoolean couple,ViInt16 peakRange,
                                              ViReal64 deltaY)
{
    ViStatus AVPA_status = VI_SUCCESS; 
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
   
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;

    if (AVPA_invalidViBooleanRange (continuousSearch))             
        return VI_ERROR_PARAMETER2;
       
    if (AVPA_invalidViBooleanRange (signalTrack))             
        return VI_ERROR_PARAMETER3;
    
    if (AVPA_invalidViBooleanRange (couple))             
        return VI_ERROR_PARAMETER4;   
       
    if (AVPA_invalidViInt16Range (peakRange,0,2))             
        return VI_ERROR_PARAMETER5;
       
    if (AVPA_invalidViReal64Range(deltaY,featuresPtr->deltaYmin,
                                         featuresPtr->deltaYmax))
        return VI_ERROR_PARAMETER6;
        
    Fmt (wrtBuffer,"%s<%s,%s,%s,%s,%s%f",s_continuousSearch[continuousSearch],
                                    s_signalTrack[signalTrack],s_peakwindow[couple],
                                    s_peakRange[peakRange],s_peakRange[3],deltaY);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Search Peak                                                     */
/* Purpose:  set execute peak marker type                                    */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Search_Peak (ViSession instrID, ViInt16 typeSearch)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (typeSearch,0,5))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuffer,"%s<%s",s_peakSearch[typeSearch]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: execute Peak to                                                 */
/* Purpose:  peak to setting                                                 */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Execute_PeakTo (ViSession instrID, ViBoolean peakTo)
{
    ViStatus AVPA_status = VI_SUCCESS; 
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (peakTo))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_peakTo[peakTo]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Marker Delta to setting                                         */
/* Purpose:  marker delta to setting                                         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Set_MarkerDeltaTo (ViSession instrID, ViInt16 deltaTo)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    if (AVPA_invalidViInt16Range (deltaTo,0,3))             
        return VI_ERROR_PARAMETER2;

    Fmt (wrtBuffer,"%s<%s",s_markerDeltaTo[deltaTo]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Take Sweep                                                      */
/* Purpose:  Sets sweep mode & starts sweeping                               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Take_Sweep (ViSession instrID, ViInt16 startSweep)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
  
    if (AVPA_invalidViInt16Range (startSweep,0,3))             
        return VI_ERROR_PARAMETER2;
     
    Fmt (wrtBuffer,"%s<%s",s_sweepMode[startSweep]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;   
       
    return AVPA_status;
}                          

/*===========================================================================*/
/* Function: Execute Trace Math                                              */
/* Purpose:  Trace Math execution                                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Execute_TraceMath (ViSession instrID, ViInt16 traceMath)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (traceMath,0,4))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (wrtBuffer,"%s<%s",s_traceMath[traceMath]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;   
 
    return AVPA_status;  
}                        

/*===========================================================================*/
/* Function: Start/Stop Trace Average                                        */
/* Purpose:  Starts stops trace average                                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Control_TraceAverage (ViSession instrID, ViInt16 traceAverage)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (traceAverage,0,3))             
        return VI_ERROR_PARAMETER2;
        
    Fmt (wrtBuffer,"%s<%s",s_traceAverage[traceAverage]);   
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;    
        
    return AVPA_status;  
}                        

/*===========================================================================*/
/* Function: Read Frequency Settings                                         */
/* Purpose:  Reads a frequency that you have chosen                          */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_FreqSettings (ViSession instrID, ViInt16 parameter,
                                                             ViPReal64 value)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (parameter,0,3))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (Buffer,"%s<%s",s_readFreq[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f",value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Frequency Offset                                           */
/* Purpose:  returns the value (unit is Hz) of the frequency offset and      */
/*           says whether this offset is active or not                       */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_FrequencyOffset (ViSession instrID,ViPReal64 value,
                                                               ViPInt16 status)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    Fmt (Buffer,"%s<%s",s_readFrequencyOffset[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    *status = featuresPtr->freqOffsetStatus;
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Frequency Step Size                                        */
/* Purpose:  returns the frequency step size and says whether this parameter */
/*           is controled  by the user or by the analyzer                    */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_FreqStepSize (ViSession instrID, ViPReal64 value,
                                          ViPInt16 controlType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readFreqStepSize[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f",value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (Buffer,"%s<%s",s_readFreqStepSize[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
          
    if (Scan(Buffer,"%s>%d[b2]", controlType)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
      
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Frequency Counter                                          */
/* Purpose:  returns the frequency counter value and says whether the counter*/
/*           is active or inactive                                           */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_FreqCounter (ViSession instrID,ViPReal64 counterValue,
                                         ViPInt16 counterStatus)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readFreqCounter[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", counterStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (Buffer,"%s<%s",s_readFreqCounter[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
          
    if (Scan(Buffer,"%s>%f", counterValue)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
      
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read  Reference Level                                           */
/* Purpose:  returns the value (without unit) of the reference level         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_ReferenceLevel(ViSession instrID, ViPReal64 refLevelValue)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readRefLevel[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", refLevelValue)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Reference Level Offset                                     */
/* Purpose:  returns the value (unit is always dB) of the reference level    */
/* offset and says whether this offset is active or not                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_RefLevelOffset (ViSession instrID,ViPReal64 value,
                                                              ViPInt16 activation)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    Fmt (Buffer,"%s<%s",s_readRefLevelOffset[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    *activation = featuresPtr->refLevelOffsetStatus;
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Reference Unit                                             */
/* Purpose:  returns the reference unit                                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_RefUnit (ViSession instrID,ViPInt16 unitNumber,
                                     ViChar _VI_FAR unitName[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readRefUnit[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", unitNumber)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(unitName,"%s<%s", s_readUnitName[*unitNumber]);  
     
    return AVPA_status;
}                         

/*===========================================================================*/
/* Function: Read Scale                                                      */
/* Purpose:  reads the scale  status number and text                         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_Scale (ViSession instrID, ViPInt16 status,
                                   ViChar _VI_FAR scaleName[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
        
    *status = featuresPtr->scale;
    
    Fmt(scaleName,"%s<%s", s_readScale[*status + 2]);
    
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read Attenuation                                                */
/* Purpose:  returns the input attenuation or the minimum input attenuation  */
/*           and says whether this parameter is controled  by the user or by */
/*           the analyzer (only for the input attenuation)                   */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_Attenuation (ViSession instrID, ViBoolean parameter,
                                         ViPReal64 value, ViPInt16 controlType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViBooleanRange (parameter))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (Buffer,"%s<%s",s_readAtt[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    if (parameter == VI_ON) {
        Fmt (Buffer,"%s<%s",s_readAtt[parameter+2]);   
        if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    
        if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
            return AVPA_status;
          
        if (Scan(Buffer,"%s>%d[b2]", controlType)!=1) { 
            AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
            return AVPA_status;
        }
    }  
    else {
        *controlType = featuresPtr->minAttControl;
    }
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read RBW and VBW                                                */
/* Purpose:  reads the resolution or video bandwith and says whether this    */
/*           parameter is controlled by the analyzer or by the user          */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_RBWandVBW (ViSession instrID, ViBoolean parameter,
                                       ViPReal64 value, ViPInt16 controlType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (parameter))             
        return VI_ERROR_PARAMETER2;
    
    Fmt (Buffer,"%s<%s",s_readRBWandVBW[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", value)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (Buffer,"%s<%s",s_readRBWandVBW[parameter+2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
        
    if (Scan(Buffer,"%s>%d[b2]", controlType)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    } 
     
    return AVPA_status;
}                        

/*===========================================================================*/
/* Function: Read  Sweep Time                                                */
/* Purpose:  reads the sweep time value and says whether we are in auto or   */
/*           manual control                                                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_SweepTime (ViSession instrID, ViPReal64 timeValue,
                                                          ViPInt16 controlType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readSweepTime[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f",timeValue)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (Buffer,"%s<%s",s_readSweepTime[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", controlType)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    } 
     
    return AVPA_status;
}                                                       

/*===========================================================================*/
/* Function: Read  Sweep Mode                                                */
/* Purpose:  returns the sweep Mode: Normal&Full, Single&Full, Normal&Window */
/*           or Single&Window                                                */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_SweepMode (ViSession instrID, ViPInt16 status,
                                       ViChar _VI_FAR sweepMode[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readSweepMode[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    if (*status==0)
        Fmt(sweepMode,"%s<%s",s_readSweepMode[1]);
    else if (*status==20)
        Fmt(sweepMode,"%s<%s",s_readSweepMode[2]);   
    else if (*status==1)
        Fmt(sweepMode,"%s<%s",s_readSweepMode[3]); 
    else if (*status==21)
        Fmt(sweepMode,"%s<%s",s_readSweepMode[4]);
     
    return AVPA_status;
}                           

/*===========================================================================*/
/* Function: Read Trigger Mode                                               */
/* Purpose:  returns the Trigger Mode: Free Run, Line, Video and external    */
/*           and the level of the video trigger                              */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_TriggerMode (ViSession instrID, ViPInt16 status,
                                         ViChar _VI_FAR triggerMode[], ViPInt16 level)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readTriggerMode[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    if (*status==0)
        Fmt(triggerMode,"%s<%s",s_readTriggerMode[2]);
    else if (*status==1)
        Fmt(triggerMode,"%s<%s",s_readTriggerMode[3]);   
    else if (*status==2)
        Fmt(triggerMode,"%s<%s",s_readTriggerMode[4]); 
    else if (*status==5)
        Fmt(triggerMode,"%s<%s",s_readTriggerMode[5]);
       
    Fmt (Buffer,"%s<%s",s_readTriggerMode[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 10, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", level)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   
     
    return AVPA_status;
}   

/*===========================================================================*/
/* Function : Read Gated Sweep                                               */
/* Purpose : returns the features of the gated sweep function                */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_GatedSweep (ViSession instrID,ViPInt16 status,
                                        ViChar _VI_FAR message[],
                                        ViPReal64 gatePosition, ViPReal64 gateWidth,
                                        ViPInt16 sourceStatus,
                                        ViChar _VI_FAR sourceType[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    
    /*read the status of the gated sweep*/
    
    Fmt (Buffer,"%s<%s",s_readGatedSweep[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(message,"%s<%s",s_textONOFF[*status]);
    
    /*reads the gate position*/
    
    Fmt (Buffer,"%s<%s",s_readGatedSweep[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", gatePosition)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /*reads the gate width*/
    
    Fmt (Buffer,"%s<%s",s_readGatedSweep[2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", gateWidth)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /*reads the gate source*/
    
    Fmt (Buffer,"%s<%s",s_readGatedSweep[3]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", sourceStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(sourceType,"%s<%s",s_readGateSource[*sourceStatus]);
    
    return AVPA_status;
}    

/*===========================================================================*/
/* Function: Read Display Line                                               */
/* Purpose:  returns the position and the status of the Display Line         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_DisplayLine(ViSession instrID, ViPReal64 level,
                                        ViPInt16 status,ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;

    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    Fmt (Buffer,"%s<%s",s_readDisplayLine[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", level)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    *status = featuresPtr->DLstatus;
    Fmt(message,"%s<%s",s_textONOFF[*status]);
    
    return AVPA_status;
}   

/*===========================================================================*/
/* Function : Read Limit Line Options                                        */
/* Purpose : returns the options used with the limit lines                   */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_LimitLineOptions (ViSession instrID, ViBoolean parameter,
                               ViPInt16 levelStatus, ViChar _VI_FAR levelMessage[],
                               ViPReal64 shiftLevel, ViPReal64 shiftTF)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (parameter))             
        return VI_ERROR_PARAMETER2;
       
    Fmt (Buffer,"%s<%s",s_readLLoptions[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", levelStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(levelMessage,"%s<%s",s_readLLmessage[*levelStatus]);
    
    Fmt (Buffer,"%s<%s",s_readLLoptions[2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", shiftLevel)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (Buffer,"%s<%s",s_readLLoptions[3]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", shiftTF)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    } 
    
    return AVPA_status;
}   

/*===========================================================================*/
/* Function: Read Measurement Window                                         */
/* Purpose:  returns the features and the status of the measurement window   */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MeasurementWindow (ViSession instrID,ViPReal64 windowPos,
                                               ViPReal64 windowWidth,ViPInt16 status,
                                               ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;

    /*reads the status of the measurement window*/
    
    Fmt (Buffer,"%s<%s",s_readMeasWindow[2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(message,"%s<%s",s_textONOFF[*status]);
    
    /*reads the position of the window*/
    
    Fmt (Buffer,"%s<%s",s_readMeasWindow[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", windowPos)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /*reads the width of the window*/ 
     
    Fmt (Buffer,"%s<%s",s_readMeasWindow[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", windowWidth)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   
    
    if (*status == 0) { /*measurement window inactive*/ 
        Fmt (Buffer,"%s<%s",s_showWindow[0]);   
        if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
            return AVPA_status;
    } 
      
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Peak Options                                               */
/* Purpose:  reads the features of the peak search                           */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_Peak_Options (ViSession instrID, ViPInt16 contPeakStatus,
                                          ViChar _VI_FAR contPeakMessage[],
                                          ViPInt16 signalTrackStatus,
                                          ViChar _VI_FAR signalTrackMessage[],
                                          ViPInt16 coupleStatus,
                                          ViChar _VI_FAR coupleMessage[],
                                          ViPReal64 deltaY)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    /*reads the status of the continuous peak search*/
    
    Fmt (Buffer,"%s<%s",s_readPeakOptions[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", contPeakStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(contPeakMessage,"%s<%s",s_textONOFF[*contPeakStatus]);
    
    /*reads the status of the signal track*/
    
    Fmt (Buffer,"%s<%s",s_readPeakOptions[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", signalTrackStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(signalTrackMessage,"%s<%s",s_textONOFF[*signalTrackStatus]);
    
    /*read the status of the coupling with the window*/
    
    Fmt (Buffer,"%s<%s",s_readPeakOptions[2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", coupleStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(coupleMessage,"%s<%s",s_textONOFF[*coupleStatus]);
    
    /*reads the value of deltaY*/
    
    Fmt (Buffer,"%s<%s",s_readPeakOptions[3]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
        
    if (Scan(Buffer,"%s>%f", deltaY)!=1) {                                  
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    return AVPA_status;
}      

/*===========================================================================*/
/* Function: Read Trace Status                                               */
/* Purpose:  returns the status of the trace A or B                          */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_TraceStatus (ViSession instrID, ViBoolean parameter,
                                         ViPInt16 status, ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViInt16 status1=0;
    ViByte Buffer[BUFFER_SIZE];
    
    if (AVPA_invalidViBooleanRange (parameter))             
        return VI_ERROR_PARAMETER2; 
 
    Fmt (Buffer,"%s<%s",s_readTraceStatus[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    if ((*status/16)>=1)  status1 = (*status/16)+5;
    else status1 = *status;
     
    Fmt(message,"%s<%s",s_readTraceMessage[status1+parameter*3]); 
    
    return AVPA_status; 
}

/*===========================================================================*/
/* Function: Read Detection Mode                                             */
/* Purpose:  returns the detector mode                                       */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_DetectionMode (ViSession instrID, ViPInt16 status,
                                           ViChar _VI_FAR detectorMode[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readDetectorMode[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(detectorMode,"%s<%s",s_readDetectorMode[*status+1]); 
    
    return AVPA_status; 
}

/*===========================================================================*/
/* Function : Read Trace Points                                              */
/* Purpose : returns the number of points in the trace                       */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_TracePoints(ViSession instrID, ViPInt16 nbPoints)
{
    ViStatus AVPA_status = VI_SUCCESS;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    *nbPoints = featuresPtr->tracePoints;
    
    return AVPA_status; 
}    

/*===========================================================================*/
/* Function: Read Limit Line Status                                          */
/* Purpose:  returns the status of the Limit Line                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_LimitLineStatus (ViSession instrID, ViBoolean parameter,
                                             ViPInt16 status,
                                             ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    
    if (AVPA_invalidViBooleanRange (parameter))             
        return VI_ERROR_PARAMETER2; 
 
    Fmt (Buffer,"%s<%s",s_readLLstatus[parameter]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(message,"%s<%s",s_textONOFF[*status]); 
    
    return AVPA_status;  
}    

/*===========================================================================*/
/* Function: Limit Line Judgment                                             */           
/* Purpose:  returns if the test with the limit line(s) is OK                */
/*===========================================================================*/    
ViStatus _VI_FUNC AVPA_Read_LLjudgment (ViSession instrID, ViPInt16 result,
                                                           ViChar _VI_FAR message[]) 
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_LLjudgement[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
        
    if (Scan(Buffer,"%s>%d[b2]", result)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(message,"%s<%s",s_LLjudgementText[*result]); 
    
    return AVPA_status; 
}

/*===========================================================================*/
/* Function: Read Level Correction Status                                    */
/* Purpose:  returns the level correction  status                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_LevelCorrectionStatus (ViSession instrID,ViPInt16 status,
                                                   ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    
    Fmt (Buffer,"%s<%s",s_readLevelCorrection[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    Fmt(message,"%s<%s",s_textONOFF[*status]); 
    
    return AVPA_status;     
} 

/*===========================================================================*/
/* Function: Read Marker Status                                              */
/* Purpose:  reads the marker status number -> text                          */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MarkerStatus (ViSession instrID, ViPInt16 status,
                                          ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_mkrNDsearch[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]",status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   
    
    Fmt(message,"%s<%s", s_mkrNDstatus[*status]);  
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Marker Delta Options                                       */
/* Purpose:  returns the options status of the marker delta                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MarkerDeltaOptions (ViSession instrID,
                                                ViPInt16 fixedMarkerStatus,
                                                ViChar _VI_FAR fixedMarkerMessage[],
                                                ViPInt16 displayModeStatus,
                                                ViChar _VI_FAR displayMode[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount=0;
    ViByte Buffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    Fmt (Buffer,"%s<%s",s_readMKDoptions[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", fixedMarkerStatus)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(fixedMarkerMessage,"%s<%s",s_textONOFF[*fixedMarkerStatus]);
    
    *displayModeStatus = featuresPtr->displayMode;
    
    Fmt(displayMode,"%s<%s",s_readDisplayMode[*displayModeStatus]);  
    
    return AVPA_status; 
}    

/*===========================================================================*/
/* Function: Read Marker Trace Status                                        */
/* Purpose:  returns the trace on which the markers are placed               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MarkerTraceStatus (ViSession instrID,ViPInt16 status,
                                               ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    
    Fmt (Buffer,"%s<%s",s_readMarkerTraceStatus[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]", status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt(message,"%s<%s",s_readMarkerTraceStatus[*status+1]); 
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Multi Marker Status                                        */
/* Purpose:  reads the multi marker status & gives text message              */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MultimarkersStatus (ViSession instrID,ViPInt16 status,
                                                ViChar _VI_FAR message[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_readmultiMkrStatus[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b2]",status)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   
    
    Fmt(message,"%s<%s", s_multiMkrMessage[*status]);  
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Trace Data                                                 */
/* Purpose:  returns trace A or B in an array                                */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_TraceData (ViSession instrID, ViBoolean trace,
                                       ViBoolean format,ViInt16 _VI_FAR traceArray[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    AVPA_ptrFeatures featuresPtr;
    ViInt16 i;
    ViUInt32 retCount;
    ViByte dataBuffer[2005];
    ViByte wrtBuffer[BUFFER_SIZE];
    
    if (AVPA_invalidViBooleanRange (trace))             
        return VI_ERROR_PARAMETER2;
        
    if (AVPA_invalidViBooleanRange (format))             
        return VI_ERROR_PARAMETER3;
        
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
       
    for (i=0;i<2005;i++) {
        dataBuffer[i]=0;
    } 
       
    if (format == VI_ON) { /*Binary Transfer*/
        Fmt (wrtBuffer,"%s<%s",s_traceBinary[trace]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
          
        if ((AVPA_status = viRead (instrID,dataBuffer,
                                    (2*featuresPtr->tracePoints)+2,&retCount)) < 0)
            return AVPA_status;
       
        for (i=0;i<featuresPtr->tracePoints;i++) {
            traceArray[i]=256*dataBuffer[2*i]+dataBuffer[2*i+1] ;
        }
    }
    
    else { /*ASCII Transfer*/ 
        Fmt (wrtBuffer,"%s<%s",s_traceASCII[trace]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        for (i=0;i<featuresPtr->tracePoints;i++) {
            if ((AVPA_status = viRead (instrID, dataBuffer, 7, &retCount)) < 0)
                return AVPA_status;
            if (Scan(dataBuffer,"%s>%d[b2]", &traceArray[i])!=1) { 
                AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
                return AVPA_status;
            }   
        }
    }   
     
  return AVPA_status; 
}

/*===========================================================================*/
/* Function: read marker normal                                              */
/* Purpose:  reads the marker position and level (without unit)              */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MarkerNormal (ViSession instrID,
                                          ViPReal64 markerNormalPosition,
                                          ViPReal64 markerNormalLevel)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    /*reads the position of the marker normal*/
    
    Fmt (Buffer,"%s<%s",s_mkrNDsearch[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", markerNormalPosition)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /*reads the level of the marker normal*/ 
     
    Fmt (Buffer,"%s<%s",s_mkrNDsearch[2]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", markerNormalLevel)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }

    return AVPA_status;
}

/*===========================================================================*/
/* Function: read marker delta                                               */
/* Purpose:  reads the position and level (without unit) of the marker delta */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MarkerDelta (ViSession instrID, 
                                         ViPReal64 markerDeltaPosition,
                                         ViPReal64 markerDeltaLevel)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViInt16 offset;
    
    /* search of the marker delta position */
    
    Fmt (Buffer,"%s<%s",s_multiMkrSearch[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;

    if ((AVPA_status = viRead (instrID, Buffer, 225, &retCount)) < 0)
        return AVPA_status;
       
    offset = (short)(retCount-21 );
    
    if (Scan(Buffer,"%s[i*]>%f",offset,markerDeltaPosition)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /* search of the marker delta level */
    
    Fmt (Buffer,"%s<%s",s_multiMkrSearch[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    if ((AVPA_status = viRead (instrID, Buffer, 225, &retCount)) < 0)
        return AVPA_status;
    
    offset = (short)(retCount - 21);
    
    if (Scan(Buffer,"%s[i*]>%f",offset,markerDeltaLevel)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   

    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Multi Markers frequencies                                  */
/* Purpose:  reads the 10 multi markers frequencies and the delta marker     */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_Multimarkers (ViSession instrID,
                                          ViReal64 _VI_FAR multiMkrPositions[10],
                                          ViPReal64 mkrDeltaPosition,
                                          ViReal64 _VI_FAR multiMkrLevels[10],
                                          ViPReal64 mkrDeltaLevel)
                                          
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    /* search of the multimarkers frequencies */
    
    Fmt (Buffer,"%s<%s",s_multiMkrSearch[0]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;

    if ((AVPA_status = viRead (instrID, Buffer, 225, &retCount)) < 0)
        return AVPA_status;
    
    if (Scan(Buffer,"%s>%10f[x]%f",multiMkrPositions,mkrDeltaPosition)!=2) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    /* search of the multimarkers levels*/
    
    Fmt (Buffer,"%s<%s",s_multiMkrSearch[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
       
    if ((AVPA_status = viRead (instrID, Buffer, 225, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%10f[x]%f",multiMkrLevels,mkrDeltaLevel)!=2) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }   

    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read MultiMarkerActive                                          */
/* Purpose:  searches the active Multi-Marker                                */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_MultiMarkerActive (ViSession instrID, ViPInt16 index)
                                      
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    ViInt16 i=0;
    ViReal64 mkrNormalPosition;
    ViReal64 multiMkrPositions[10];
    ViReal64 mkrDeltaPosition;
    ViReal64 multiMkrLevels[10];
    ViReal64 mkrDeltaLevel;
    
    
    Fmt (Buffer,"%s<%s",s_mkrNDsearch[1]);   
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%f", &mkrNormalPosition)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    AVPA_Read_Multimarkers (instrID, multiMkrPositions,&mkrDeltaPosition,
                                     multiMkrLevels,&mkrDeltaLevel);
    
    while ( (i<10)&&((multiMkrPositions[i]!=mkrNormalPosition)||(multiMkrLevels[i]==0)) ) {
        i++;
    } 
    
    *index = i+1;
    
    return AVPA_status; 
}                                                                                                



/*===========================================================================*/
/* Function: Write To Instrument                                             */
/* Purpose:  This function writes a command string to the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_WriteInstrData (ViSession instrID, ViString writeBuffer)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (wrtBuffer,"%s<%s",writeBuffer);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status; 
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Read Instr Data                                                 */
/* Purpose:  This function reads the output buffer of the instrument.        */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_readInstrData (ViSession instrID, ViInt32 numBytes,
                                      ViChar _VI_FAR readBuffer[],ViPInt32 bytesRead)
{
    ViStatus AVPA_status = VI_SUCCESS;
    *bytesRead = 0L;
        
    if ((AVPA_status = viRead (instrID, (ViPBuf)readBuffer, numBytes, (ViPUInt32)bytesRead)) < 0)
        return AVPA_status;

    return AVPA_status;
}

/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function resets the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_reset (ViSession instrSession)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    AVPA_ptrFeatures featuresPtr;
    
    if ((AVPA_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &featuresPtr)) < 0)
        return AVPA_status;

    /*  Initialize the instrument to a known state.  */
    Fmt (wrtBuffer,"%s<%s",s_reset[0]);     
    if ((AVPA_status = viWrite(instrSession,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
   
    if ((AVPA_status = AVPA_defaultInstrSetup (instrSession, "NO", featuresPtr->instrumentType)) < 0)  
        return AVPA_status;
        
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Self-Test                                                       */
/* Purpose:  This function executes the instrument self-test and returns     */
/*           the result. If the self test function is not supported by the   */
/*           instrument, this function returns the warning                   */
/*           VI_WARN_NSUP_SELF_TEST. (Self Test not supported)               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_selfTest (ViSession instrID, ViPInt16 selfTestResult,
                                                    ViChar _VI_FAR selfTestMessage[])
{
    ViStatus AVPA_status = VI_WARN_NSUP_SELF_TEST;
    
    Fmt (selfTestMessage, "%s<Self-Test Not Supported");
    *selfTestResult = -1;
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Error Query                                                     */
/* Purpose:  This function queries the instrument error queue, and returns   */
/*           the result.                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_errorQuery (ViSession instrID, ViPInt32 errorCode,
                                   ViChar _VI_FAR errorMessage[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (Buffer,"%s<%s",s_errorQuery[0]);     
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    if ((AVPA_status = viRead (instrID, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%d[b4]", &errorCode)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (errorMessage,"%s<%s",""); 

    return AVPA_status;
}

/*===========================================================================*/
/* Function: Error Message                                                   */
/* Purpose:  This function translates the error return value from the        */
/*           instrument driver into a user-readable string.                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_errorMessage (ViSession instrID, ViStatus errorCode,
                                     ViChar _VI_FAR errorMessage[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViInt16 i;
    
    static AVPA_tStringValPair statusDescArray[] = 
    {
        {VI_WARN_NSUP_SELF_TEST,    "WARNING: Self-test not supported"},
        {VI_ERROR_PARAMETER1,        "ERROR: Parameter 1 out of range"},
        {VI_ERROR_PARAMETER2,        "ERROR: Parameter 2 out of range"},
        {VI_ERROR_PARAMETER3,        "ERROR: Parameter 3 out of range"},
        {VI_ERROR_PARAMETER4,        "ERROR: Parameter 4 out of range"},
        {VI_ERROR_PARAMETER5,        "ERROR: Parameter 5 out of range"},
        {VI_ERROR_PARAMETER6,        "ERROR: Parameter 6 out of range"},
        {VI_ERROR_PARAMETER7,        "ERROR: Parameter 7 out of range"},
        {VI_ERROR_PARAMETER8,        "ERROR: Parameter 8 out of range"},
        {VI_ERROR_INSTR_PARAMETER9 , "ERROR: Parameter 9 out of range"},
        {VI_ERROR_INSTR_PARAMETER10, "ERROR: Parameter 10 out of range"},
        {VI_ERROR_INSTR_PARAMETER11, "ERROR: Parameter 11 out of range"},
        {VI_ERROR_INSTR_PARAMETER12, "ERROR: Parameter 12 out of range"},
        {VI_ERROR_INSTR_PARAMETER13, "ERROR: Parameter 13 out of range"},
        {VI_ERROR_INSTR_PARAMETER14, "ERROR: Parameter 14 out of range"},
        {VI_ERROR_INSTR_PARAMETER15, "ERROR: Parameter 15 out of range"},
        {VI_ERROR_FAIL_ID_QUERY,    "ERROR: Identification query failed"},
        {VI_ERROR_INSTR_FILE_OPEN,  "ERROR: Opening the specified file"},
        {VI_ERROR_INSTR_FILE_WRITE, "ERROR: Writing to the specified file"},
        {VI_ERROR_INSTR_INTERPRETING_RESPONSE, "ERROR: Interpreting the instrument's response"},
        {VI_ERROR_MARKERINACTIVE,   "ERROR: The chosen marker is actually inactive"}, 
        {VI_NULL, VI_NULL}
     };

    AVPA_status = viStatusDesc (instrID, errorCode, errorMessage);
    
    if (AVPA_status == VI_WARN_UNKNOWN_STATUS) {
        for (i=0; statusDescArray[i].stringName; i++) {
            if (statusDescArray[i].stringVal == errorCode) {
                Fmt (errorMessage, "%s<%s", statusDescArray[i].stringName);
                return (VI_SUCCESS);
            }
        }
        Fmt (errorMessage, "%s<Unknown Error 0x%x[uw8p0]", errorCode); 
        return (VI_WARN_UNKNOWN_STATUS);
    }
    
    AVPA_status = VI_SUCCESS;
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Revision Query                                                  */
/* Purpose:  This function returns the driver and instrument revisions.      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_revisionQuery (ViSession instrID, ViChar _VI_FAR driverRev[],
                                      ViChar _VI_FAR instrRev[])
{
    ViUInt32 retCount = 0;
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte Buffer[BUFFER_SIZE];
    
    Fmt (Buffer,"%s<%s",s_instrRevision[0]);     
    if ((AVPA_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
    
    if ((AVPA_status = viRead (instrID, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
       
    if (Scan(Buffer,"%s>%s", instrRev)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
     
    Fmt (driverRev, "%s<%s", AVPA_REVISION);
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Calibration                                                     */
/* Purpose:  Set calibration to element                                      */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Calibration (ViSession instrID, ViInt16 element,
                                    ViReal64 level, ViBoolean calCorrection,
                                    ViBoolean freqCorrection)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViUInt32 retCount=0;
    
    if ((AVPA_status = viGetAttribute (instrID,VI_ATTR_USER_DATA,&featuresPtr)) < 0)
        return AVPA_status;
    
    if (AVPA_invalidViInt16Range (element,0,7))             
        return VI_ERROR_PARAMETER2;
        
    if (AVPA_invalidViReal64Range (level,featuresPtr->calLevelMin,
                                        featuresPtr->calLevelMax))          
        return VI_ERROR_PARAMETER3;

    if (AVPA_invalidViBooleanRange (calCorrection))             
        return VI_ERROR_PARAMETER4;
    
    if (AVPA_invalidViBooleanRange (freqCorrection))             
        return VI_ERROR_PARAMETER5;
       
    /* element to calibrate*/  
    Fmt (wrtBuffer,"%s<%s%f,%s",s_calLevel[0],level,s_calibration[element]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;     

    /* calibration and frequency correction */ 
    Fmt (wrtBuffer,"%s<%s,%s",s_freqCorrection[freqCorrection],
                              s_calCorrection[calCorrection]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
    
    return AVPA_status;
}


/*===========================================================================*/
/* Function: Display label                                                   */
/* Purpose:  selects label                                                   */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Display_Label (ViSession instrID, ViBoolean showLabel,
                                      ViString label)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (showLabel))             
        return VI_ERROR_PARAMETER2;
       
    if (showLabel == VI_ON) {
        Fmt (wrtBuffer,"%s<%s%s/",s_label[showLabel],label);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status; 
    }
    else {
        Fmt (wrtBuffer,"%s<%s",s_label[showLabel]);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    
    return AVPA_status;   
} 

/*===========================================================================*/
/* Function: Copy screen                                                     */
/* Purpose:  Screen copy                                                     */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Copy_Screen (ViSession instrID, ViInt16 copyMode)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (copyMode,0,2))             
        return VI_ERROR_PARAMETER2;
       
    Fmt (wrtBuffer,"%s<%s",s_copyScreen[copyMode]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;     
        
    return AVPA_status; 
}

/*===========================================================================*/
/* Function : Read Status Byte                                               */
/* purpose : controls the status byte register                               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Read_StatusByte (ViSession instrID, ViPInt16 statusByte)
{
    ViStatus AVULIS_status = VI_SUCCESS;
    ViUInt16 status = 0;
    
    AVULIS_status = viReadSTB (instrID, &status);
    * statusByte = (ViInt16) status;
    
    return AVULIS_status;
}

/*===========================================================================*/
/* Function : Clear Status Byte                                              */
/* purpose : clears the status byte register                                 */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Clear_StatusByte (ViSession instrID)
{
 
    ViStatus AVULIS_status = VI_SUCCESS;
    ViUInt32 retCount = 0;
    ViByte Buffer[BUFFER_SIZE];
    
    Fmt (Buffer,"%s<%s",s_clearStatusByte[0]);     
    if ((AVULIS_status = viWrite(instrID,Buffer,NumFmtdBytes(),&retCount)) < 0)
        return AVULIS_status;
       
    return AVULIS_status;
    
}

/*===========================================================================*/
/* Function : Convert Frequency                                              */
/* Purpose : converts the frequency value returned in Hz by the analyzer     */
/*           in the value with the best unit (Hz, kHz, MHz or GHz)           */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Convert_Frequency (ViSession instrID,ViReal64 inputValue,
                                          ViPReal64 outputValue,
                                          ViChar _VI_FAR frequencyUnit[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViReal64 denom[]={1,1000,1000000,1000000000},interim[3]={0};
    ViInt16 i;
    
    for (i=0;i<3;i++) {
        interim[i] = inputValue / denom[i+1];
    }
    
    i=0; 
    while ( (i<3)&&(fabs(interim[i])>=1) ) {
        i++;
    }
     
    *outputValue = inputValue / denom[i];
    
    Fmt(frequencyUnit,"%s<%s",s_freqUnit[i]); 
    
    return AVPA_status;
} 

/*===========================================================================*/
/* Function : Convert Time                                                   */
/* Purpose : converts the time value returned in second(s) by the analyzer   */
/*           in the value with the best unit (ns, us, ms or s)               */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Convert_Time (ViSession instrID, ViReal64 inputValue,
                                     ViPReal64 outputValue,
                                     ViChar _VI_FAR timeUnit[])
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViReal64 compare[]={1000000000,1000000,1000,1};
    ViInt16 i;
    
    i=0;
    while ((i<=2)&&(fabs(inputValue)*compare[i]>=1000)) {
        i++;
    }
     
    *outputValue = inputValue*compare[i];
    Fmt(timeUnit,"%s<%s",s_timeUnit[i]);
    
    return AVPA_status;
}  

/*===========================================================================*/
/* Function: Initialize card                                                 */
/* Purpose:  Initialize card on drive A or B                                 */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Init_MemoryCard (ViSession instrID, ViBoolean drive)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (drive))          
        return VI_ERROR_PARAMETER2;
       
    Fmt (wrtBuffer,"%s<%s",s_initCard[drive]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;      
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: select Memory Card                                              */
/* Purpose:  selects memory card drive                                       */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Select_MemoryDrive (ViSession instrID, ViInt16 drive)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViInt16Range (drive,1,3))
        return VI_ERROR_PARAMETER2;
       
    Fmt (wrtBuffer,"%s<%s",s_selectCard[drive - 1]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;    
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Copy Memory Card                                                */
/* Purpose:  Copy card A: to card B:                                         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Copy_MemoryCard (ViSession instrID)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (wrtBuffer,"%s<%s",s_copyCard[0]);     
    if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status; 
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: recall a file                                                   */
/* Purpose:  select the register or the file name to recall                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Recall_File (ViSession instrID, ViBoolean recallType,
                                    ViInt16 registerName, ViString fileName)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (recallType))             
        return VI_ERROR_PARAMETER2; 
    
    if (recallType == VI_ON) { /*select file name*/     
        Fmt (wrtBuffer,"%s<%s%s/",s_recallFile[recallType],fileName);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status; 
    }
    else { /*select register*/   
    	if (AVPA_invalidViInt16Range (registerName,1,10))          
        	return VI_ERROR_PARAMETER3;
        if (registerName==10) {
        	Fmt (wrtBuffer,"%s<%s%d[b2]/",s_recallFile[recallType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
        }
        else {
        	Fmt (wrtBuffer,"%s<%s0%d[b2]/",s_recallFile[recallType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
        }
    }
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: save a file                                                     */
/* Purpose:  select the register or the file name to save                    */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Save_File (ViSession instrID, ViBoolean saveType,
                                  ViInt16 registerName, ViString fileName)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (saveType))             
        return VI_ERROR_PARAMETER2; 
    
    if (saveType == VI_ON) { /*select file name*/   
        Fmt (wrtBuffer,"%s<%s%s/",s_saveFile[saveType],fileName);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status; 
    }
    else { /*select register*/ 
    	if (AVPA_invalidViInt16Range (registerName,1,10))          
        	return VI_ERROR_PARAMETER3;
        if (registerName==10) {
        	Fmt (wrtBuffer,"%s<%s%d[b2]/",s_saveFile[saveType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
        }
        else {
        	Fmt (wrtBuffer,"%s<%s0%d[b2]/",s_saveFile[saveType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
       }     	
    }
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: delete a file                                                   */
/* Purpose:  select the register or the file name to delete                  */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_Delete_File (ViSession instrID, ViBoolean deleteType,
                                    ViInt16 registerName, ViString fileName)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    if (AVPA_invalidViBooleanRange (deleteType))             
        return VI_ERROR_PARAMETER2; 
    
    if (deleteType == VI_ON) { /*select file name*/   
        Fmt (wrtBuffer,"%s<%s%s/",s_deleteFile[deleteType],fileName);     
        if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status; 
    }
    else { /*select register*/ 
    	if (AVPA_invalidViInt16Range (registerName,1,10))          
        	return VI_ERROR_PARAMETER3;
        if (registerName==10) {
        	Fmt (wrtBuffer,"%s<%s%d[b2]/",s_deleteFile[deleteType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
        }
        else {
            Fmt (wrtBuffer,"%s<%s0%d[b2]/",s_deleteFile[deleteType],registerName);     
        	if ((AVPA_status = viWrite(instrID,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
            	return AVPA_status;
        }     	
    }
    
    return AVPA_status;
}

/*===========================================================================*/
/* Function: Close                                                           */
/* Purpose:  This function closes the instrument.                            */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_close (ViSession instrSession)
{
    AVPA_ptrFeatures featuresPtr;
    ViSession rmSession;
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    Fmt (wrtBuffer,"%s<%s",s_close[0]);     
    if ((AVPA_status = viWrite(instrSession,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;

    if ((AVPA_status = viGetAttribute (instrSession,VI_ATTR_RM_SESSION,&rmSession))<0)
        return AVPA_status;
    if ((AVPA_status = viGetAttribute (instrSession,VI_ATTR_USER_DATA,&featuresPtr))<0)
        return AVPA_status;
    
    if (featuresPtr != NULL) 
        free (featuresPtr);
    
    AVPA_status = viClose (instrSession);
    viClose (rmSession);

    return AVPA_status;
}  

/*****************************************************************************/
/*= UTILITY ROUTINES (Non-Exportable Functions) =============================*/
/*****************************************************************************/

/*===========================================================================*/
/* Function: Boolean Value Out Of Range - ViBoolean                          */
/* Purpose:  This function checks a Boolean to see if it is equal to VI_TRUE */
/*           or VI_FALSE. If the value is out of range, the return value is  */
/*           VI_TRUE, otherwise the return value is VI_FALSE.                */
/*===========================================================================*/
ViBoolean AVPA_invalidViBooleanRange (ViBoolean val)
{
    return ((val != VI_OFF && val != VI_ON) ? VI_ON : VI_OFF);
}

/*===========================================================================*/
/* Function: Short Signed Integer Value Out Of Range - ViInt16               */
/* Purpose:  This function checks a short signed integer value to see if it  */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range, the return value is VI_TRUE, otherwise the return     */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
ViBoolean AVPA_invalidViInt16Range (ViInt16 val, ViInt16 min, ViInt16 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Long Signed Integer Value Out Of Range - ViInt32                */
/* Purpose:  This function checks a long signed integer value to see if it   */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range,  the return value is VI_TRUE, otherwise the return    */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
ViBoolean AVPA_invalidViInt32Range  (ViInt32 val, ViInt32 min, ViInt32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Short Unsigned Integer Value Out Of Range - ViUInt16            */
/* Purpose:  This function checks a short unsigned integer value to see if it*/  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range,  the return value is VI_TRUE, otherwise the return    */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
ViBoolean AVPA_invalidViUInt16Range  (ViUInt16 val, ViUInt16 min, ViUInt16 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Long Unsigned Integer Value Out Of Range - ViUInt32             */
/* Purpose:  This function checks a long unsigned integer value to see if it */  
/*           lies between a minimum and maximum value.  If the value is out  */
/*           of range,  the return value is VI_TRUE, otherwise the return    */
/*           value is VI_FALSE.                                              */
/*===========================================================================*/
ViBoolean AVPA_invalidViUInt32Range  (ViUInt32 val, ViUInt32 min, ViUInt32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Real (Float) Value Out Of Range - ViReal32                      */
/* Purpose:  This function checks a real (float) value to see if it lies     */  
/*           between a minimum and maximum value.  If the value is out of    */
/*           range, the return value is VI_TRUE, otherwise the return value  */
/*           is VI_FALSE.                                                    */
/*===========================================================================*/
ViBoolean AVPA_invalidViReal32Range  (ViReal32 val, ViReal32 min, ViReal32 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Real (Double) Value Out Of Range - ViReal64                     */
/* Purpose:  This function checks a real (double) value to see if it lies    */  
/*           between a minimum and maximum value.  If the value is out of    */
/*           range, the return value is VI_TRUE, otherwise the return value  */
/*           is VI_FALSE.                                                    */
/*===========================================================================*/
ViBoolean AVPA_invalidViReal64Range  (ViReal64 val, ViReal64 min, ViReal64 max)
{
    return ((val < min || val > max) ? VI_TRUE : VI_FALSE);
}

/*===========================================================================*/
/* Function: Initialize Clean Up                                             */
/* Purpose:  This function is used only by the AVPA_init function.  When     */
/*           an error is detected this function is called to close the       */
/*           open resource manager and instrument object sessions and to     */
/*           set the instrSession that is returned from AVPA_init to         */
/*           VI_NULL.                                                        */
/*===========================================================================*/
ViStatus AVPA_initCleanUp (ViSession openRMSession,
                    ViPSession openInstrSession, ViStatus currentStatus)
{
    viClose (*openInstrSession);
    viClose (openRMSession);
    *openInstrSession = VI_NULL;
    
    return currentStatus;
}

/*****************************************************************************/
/*----------- INSERT INSTRUMENT-DEPENDENT UTILITY ROUTINES HERE -------------*/
/*****************************************************************************/


/*===========================================================================*/
/* Function: Default Instrument Setup                                        */
/* Purpose:  This function sends a default setup to the instrument.  This    */
/*           function is called by the AVPA_reset operation and by the       */
/*           AVPA_init function if the reset option has not been             */
/*           selected.  This function is useful for configuring any          */
/*           instrument settings that are required by the rest of the        */
/*           instrument driver functions such as turning headers ON or OFF   */
/*           or using the long or short form for commands, queries, and data.*/                                    
/*===========================================================================*/
ViStatus AVPA_defaultInstrSetup (ViSession instrSession, ViRsrc resourceName, ViUInt32 instrType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount = 0;
    ViByte cable, wrtBuffer[BUFFER_SIZE];
    AVPA_ptrFeatures featuresPtr;
    ViInt16 i,traceArray[1001];
    ViReal64 MKMpositions[10],positionDelta,MKMlevels[10],levelDelta;
    
    /* Determine if the structure has been initialized for the current VISA  */
    /* session and malloc if not.*/
    if ((AVPA_status = viGetAttribute (instrSession, VI_ATTR_USER_DATA, &featuresPtr)))
        return AVPA_status;
    
    if (featuresPtr == NULL) 
        featuresPtr = malloc (sizeof (struct AVPA_instrFeatures));
    
    /* the marker display mode is set to absolute*/
    Fmt (wrtBuffer,"%s<%s",s_displayMode[0]);     
    if ((AVPA_status = viWrite(instrSession,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
       
    /* the minimum attenuation is controled by the analyzer*/
    Fmt (wrtBuffer,"%s<%s",s_attMin[1]);     
    if ((AVPA_status = viWrite(instrSession,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;   
    
    /*sets the display line to inactive*/   
    Fmt (wrtBuffer,"%s<%s",s_showDL[0]);     
    if ((AVPA_status = viWrite(instrSession,wrtBuffer,NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
    
    
    switch (instrType) {
     
        case 3263 :
            featuresPtr->frequencyLow[0] = 0;
            featuresPtr->frequencyHigh[0] = 3e9;
            featuresPtr->frequencyLow[1] = -200e6;
            featuresPtr->frequencyHigh[1] = 3e9;
            featuresPtr->frequencyLow[2] = 0;
            featuresPtr->frequencyHigh[2] = 3.2e9;
            featuresPtr->spanLow = 0;
            featuresPtr->spanHigh = 3e9;
            break;
      
        case 3272 :
            featuresPtr->frequencyLow[0] = 0;
            featuresPtr->frequencyHigh[0] = 26.5e9;
            featuresPtr->frequencyLow[1] = -200e6;
            featuresPtr->frequencyHigh[1] = 26.5e9;
            featuresPtr->frequencyLow[2] = 0;
            featuresPtr->frequencyHigh[2] = 26.7e9;
            featuresPtr->spanLow = 0;
            featuresPtr->spanHigh = 26.5e9;
            break;
       
        case 3463 :
            featuresPtr->frequencyLow[0] = 0;
            featuresPtr->frequencyHigh[0] = 3e9;
            featuresPtr->frequencyLow[1] = -200e6;
            featuresPtr->frequencyHigh[1] = 3e9;
            featuresPtr->frequencyLow[2] = 0;
            featuresPtr->frequencyHigh[2] = 3.2e9;
            featuresPtr->spanLow = 0;
            featuresPtr->spanHigh = 3e9;
            break;
      
        case 3465 :
            featuresPtr->frequencyLow[0] = 0;
            featuresPtr->frequencyHigh[0] = 8e9;
            featuresPtr->frequencyLow[1] = -200e6;
            featuresPtr->frequencyHigh[1] = 8e9;
            featuresPtr->frequencyLow[2] = 0;
            featuresPtr->frequencyHigh[2] = 8.2e9;
            featuresPtr->spanLow = 0;
            featuresPtr->spanHigh = 8e9;
            break;
      
    }
    
    featuresPtr->instrumentType = instrType;
    featuresPtr->freqOffsetMin = -100e6;   /*MHz*/
    featuresPtr->freqOffsetMax = +100e6;
    featuresPtr->refLevelOffsetMin = -100; /*dB*/
    featuresPtr->refLevelOffsetMax = +100;
    featuresPtr->inputAttMin = 0;   /*dB*/
    featuresPtr->inputAttMax = 70;
    featuresPtr->ratio_RBW_SpanMin = 0.001;
    featuresPtr->ratio_RBW_SpanMax = 0.1;
    featuresPtr->ratio_RBW_VBWMin = 0.003;
    featuresPtr->ratio_RBW_VBWMax = 3;
    featuresPtr->sweepTimeMini = 0.05;
    featuresPtr->sweepTimeMaxi = 1000;
    featuresPtr->gatePositionMini = 1e-6;
    featuresPtr->gatePositionMaxi = 65e-3;
    featuresPtr->gateWidthMini = 2e-6;
    featuresPtr->gateWidthMaxi = 65e-3;
    featuresPtr->averageCountMini = 2;
    featuresPtr->averageCountMaxi = 999;
    featuresPtr->DL_LevelMin = -1000;
    featuresPtr->DL_LevelMax = 1000;
    featuresPtr->refLevelMin[0]=-105;
    featuresPtr->refLevelMin[1]=-58;
    featuresPtr->refLevelMin[2]=2;
    featuresPtr->refLevelMin[3]=8;
    featuresPtr->refLevelMin[4]=-15;
    featuresPtr->refLevelMin[5]=0;
    featuresPtr->refLevelMin[6]=3.16e-14;
    featuresPtr->refLevelMin[7]=1.25e-6;
    featuresPtr->refLevelMax[0]=60;
    featuresPtr->refLevelMax[1]=107;
    featuresPtr->refLevelMax[2]=167;
    featuresPtr->refLevelMax[3]=173;
    featuresPtr->refLevelMax[4]=150;
    featuresPtr->refLevelMax[5]=0;
    featuresPtr->refLevelMax[6]=1000;
    featuresPtr->refLevelMax[7]=223.6;
    featuresPtr->displayMode=0; /*displayMode->absolute*/
    featuresPtr->minAttControl=1; /*min input attenuation controled by the analyzer*/
    featuresPtr->DLstatus = 0; /*DL set to inactive*/
    featuresPtr->deltaYmin=0.1;
    featuresPtr->deltaYmax=10;
    featuresPtr->calLevelMin=-30;
    featuresPtr->calLevelMax=-10;
    
    // returns the scale
    AVPA_Search_Scale (instrSession, &featuresPtr->scale);
    
    // returns the ref level offset status
    AVPA_Search_RefOffsetStatus (instrSession, &featuresPtr->refLevelOffsetStatus);
    
    // returns the frequency offset status
    AVPA_Search_FreqOffsetStatus (instrSession, &featuresPtr->freqOffsetStatus);
    
    AVPA_Read_Multimarkers (instrSession,MKMpositions,&positionDelta,
                                         MKMlevels,&levelDelta);
    
    for (i=0;i<10;i++) {
        if ((MKMpositions[i]!=0)||(MKMlevels[i]!=0)) featuresPtr->MKRMstatus[i]=1;
        else featuresPtr->MKRMstatus[i]=0;
    } 
     
    Fmt (featuresPtr -> instrDriverRevision, "%s<%s", AVPA_REVISION);  
    
    if (viSetAttribute (instrSession, VI_ATTR_USER_DATA, (ViUInt32)featuresPtr)!= 0)
        return AVPA_status;
    
    featuresPtr->tracePoints = 1001;
    cable = resourceName[0];
    
    if (cable == 'G') {   
    
    	for (i=0;i<1001;i++) {
        	traceArray[i]=0;
    	}
    
    	AVPA_Read_TraceData (instrSession,1,1,traceArray);
    	
    	if (traceArray[501]==0) featuresPtr->tracePoints = 501; 
    	else featuresPtr->tracePoints = 1001;
    	
    }
    
    return AVPA_status;
    
}
      
/*===========================================================================*/
/* Function: Reset                                                           */
/* Purpose:  This function resets the instrument.                            */
/*===========================================================================*/
ViStatus AVPA_initReset (ViSession instrSession, ViRsrc resourceName, ViUInt32 instrType)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViByte wrtBuffer[BUFFER_SIZE];
    ViUInt32 retCount=0;
    
    /*Initialize the instrument to a known state.*/
    Fmt (wrtBuffer,"%s<%s",s_reset[0]);
    if ((AVPA_status = viWrite(instrSession, wrtBuffer, NumFmtdBytes(),&retCount)) < 0)
        return AVPA_status;
   
    if ((AVPA_status = AVPA_defaultInstrSetup (instrSession, resourceName, instrType)) < 0)  
        return AVPA_status;
        
    return AVPA_status;
}

/*===========================================================================*/
/* function  Search Scale                                                    */
/* Purpose : search the scale used by the analyzer at the beginning of the   */
/*           remote control                                                  */
/*===========================================================================*/
ViStatus AVPA_Search_Scale (ViSession instrSession, ViPInt16 scale)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    ViInt16   unitNumberStart, unitNumber;
    ViInt16   dbScale, linScale;
    ViChar unitName[5];
    
    /* returns the unit */
    AVPA_Read_RefUnit (instrSession, &unitNumberStart, unitName);
    
    /* send a new unit --> dBm */
    AVPA_Set_RefUnit (instrSession, 0);
    
    // returns the current or last log scale
    Fmt (Buffer,"%s<%s",s_readScale[0]);   
    if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
    	return AVPA_status;
    if ((AVPA_status = viRead (instrSession, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
    if (Scan(Buffer,"%s>%d[b2]", &dbScale)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    // returns the current or last lin scale
    Fmt (Buffer,"%s<%s",s_readScale[1]);   
    if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
    	return AVPA_status;
    if ((AVPA_status = viRead (instrSession, Buffer, 5, &retCount)) < 0)
        return AVPA_status;
    if (Scan(Buffer,"%s>%d[b2]", &linScale)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    /* we send the order to pass in linear scale */
    /* if we are in log scale --> lin scale and unit = V*/
    /* if we are in lin scale --> lin scale and unit = dBm*/
    Fmt (Buffer,"%s<%s",s_yScale[5]);   
    if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    AVPA_Read_RefUnit (instrSession, &unitNumber, unitName);
    if (unitNumber == 6) {  /* unit = V --> log scale */ 
        *scale = dbScale; 
        Fmt (Buffer,"%s<%s",s_yScale[dbScale]);   
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    }
    else {  /* unit = dBm --> linear scale */
        *scale = linScale + 5;
        Fmt (Buffer,"%s<%s",s_yScale[linScale + 5]);   
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    }
    
    /* send the unit used at the beginning of this function */
    AVPA_Set_RefUnit (instrSession, unitNumberStart);
        
    return AVPA_status;
} 

/*===========================================================================*/
/* function  Search Reference Level Offset Status                            */
/* Purpose : search whether the ref offset is active or not at the beginning */
/*           of the remote control                                           */
/*===========================================================================*/
ViStatus AVPA_Search_RefOffsetStatus (ViSession instrSession, ViPInt16 refOffsetStatus)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    ViReal64 refOffsetStart, refLevelStart, refLevelAfter;
    
    /* reads the reference offset value */
    Fmt (Buffer,"%s<%s",s_readRefLevelOffset[0]);   
    if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    if ((AVPA_status = viRead (instrSession, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
    if (Scan(Buffer,"%s>%f", &refOffsetStart)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    /* reads the reference level value */
    AVPA_Read_ReferenceLevel (instrSession, &refLevelStart);
    
    if (refOffsetStart == 0) {
        *refOffsetStatus = 0;
        /* deactivate the reference offset if it was active */
        Fmt (Buffer,"%s<%s", s_refLevelOffset[0]);     
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else {
        /* deactivate the reference offset */
        Fmt (Buffer,"%s<%s", s_refLevelOffset[0]);     
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        /* reads the new value of the reference level after the deactivation of the ref offset*/
        AVPA_Read_ReferenceLevel (instrSession, &refLevelAfter);
        /* status of the reference offset */
        if (refLevelAfter != refLevelStart) {  /*ref offset was active*/
            *refOffsetStatus = 1;
            Fmt (Buffer,"%s<%s%fDB", s_refLevelOffset[1], refOffsetStart);     
            if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
                return AVPA_status;
        }
        else {  /* reference offset was inactive */
            *refOffsetStatus = 0;
        }
    }   
         
    return AVPA_status;
} 

/*===========================================================================*/
/* function  Search Frequency Offset Status                                  */
/* Purpose : search whether the freq offset is active or not at the 		 */
/*           beginning of the remote control                                 */
/*===========================================================================*/
ViStatus AVPA_Search_FreqOffsetStatus (ViSession instrSession, ViPInt16 freqOffsetStatus)
{
    ViStatus AVPA_status = VI_SUCCESS;
    ViUInt32 retCount;
    ViByte Buffer[BUFFER_SIZE];
    ViReal64 freqOffsetStart, centerFreqStart, centerFreqAfter;
    
    /* reads the frequency offset value */
    Fmt (Buffer,"%s<%s",s_readFrequencyOffset[0]);   
    if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount))<0)
        return AVPA_status;
    if ((AVPA_status = viRead (instrSession, Buffer, 25, &retCount)) < 0)
        return AVPA_status;
    if (Scan(Buffer,"%s>%f", &freqOffsetStart)!=1) { 
        AVPA_status = VI_ERROR_INSTR_INTERPRETING_RESPONSE;
        return AVPA_status;
    }
    
    /* reads the center frequency value */
    AVPA_Read_FreqSettings (instrSession, 0, &centerFreqStart);
    
    if (freqOffsetStart == 0) {
        *freqOffsetStatus = 0;
        /* deactivate the frequency offset if it was active */
        Fmt (Buffer,"%s<%s", s_freqOffset[0]);     
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
    }
    else {
        /* deactivate the frequency offset */
        Fmt (Buffer,"%s<%s", s_freqOffset[0]);     
        if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
            return AVPA_status;
        /* reads the new value of the center frequency after the deactivation of the freq offset*/
        AVPA_Read_FreqSettings (instrSession, 0, &centerFreqAfter);
        /* status of the frequency offset */
        if (centerFreqAfter != centerFreqStart) {  /*freq offset was active*/
            *freqOffsetStatus = 1;
            Fmt (Buffer,"%s<%s%fHZ", s_freqOffset[1], freqOffsetStart);     
            if ((AVPA_status = viWrite(instrSession,Buffer,NumFmtdBytes(),&retCount)) < 0)
                return AVPA_status;
        }
        else {  /* reference offset was inactive */
            *freqOffsetStatus = 0;
        }
    }   
         
    return AVPA_status;
} 

/*****************************************************************************/
/*=== END INSTRUMENT DRIVER SOURCE CODE =====================================*/
/*****************************************************************************/
