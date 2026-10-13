/*= ADVANTEST PA Series Spectrum Analyzers R3263/72/3463/65 Include File ====*/

/*===========================================================================*/
/*  Please do not use global variables or arrays in the include file         */
/*  instrument drivers.                                                      */
/*===========================================================================*/
     
#ifndef __AVPA_HEADER
#define __AVPA_HEADER

#include <vpptype.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/*===========================================================================*/
/* Instrument Specific Error Codes                                           */
/*===========================================================================*/
#define VI_ERROR_INSTR_FILE_OPEN              (_VI_ERROR+0x3FFC0800L)
#define VI_ERROR_INSTR_FILE_WRITE             (_VI_ERROR+0x3FFC0801L)

#define VI_ERROR_INSTR_INTERPRETING_RESPONSE  (_VI_ERROR+0x3FFC0803L)
#define VI_ERROR_INSTR_PARAMETER9             (_VI_ERROR+0x3FFC0809L)
#define VI_ERROR_INSTR_PARAMETER10            (_VI_ERROR+0x3FFC080AL)
#define VI_ERROR_INSTR_PARAMETER11            (_VI_ERROR+0x3FFC080BL)
#define VI_ERROR_INSTR_PARAMETER12            (_VI_ERROR+0x3FFC080CL)
#define VI_ERROR_INSTR_PARAMETER13            (_VI_ERROR+0x3FFC080DL)
#define VI_ERROR_INSTR_PARAMETER14            (_VI_ERROR+0x3FFC080EL)
#define VI_ERROR_INSTR_PARAMETER15            (_VI_ERROR+0x3FFC080FL)

#define VI_WARNING_INSTR_OFFSET                         (0x3FFC0900L) 
#define VI_ERROR_INSTR_OFFSET                 (_VI_ERROR+0x3FFC0900L)

#define PREFIX_ERROR_INVALID_CONFIGURATION    (VI_ERROR_INSTR_OFFSET + 0xF0L)

#define VI_ERROR_MARKERINACTIVE               (_VI_ERROR+0x3FFC0810L)

/*===========================================================================*/
/* GLOBAL USER-CALLABLE FUNCTION DECLARATIONS (Exportable Functions)         */
/*===========================================================================*/
ViStatus _VI_FUNC AVPA_init (ViRsrc resourceName, ViBoolean IDQuery,
                      ViBoolean resetDevice, ViPSession instrID);
ViStatus _VI_FUNC AVPA_configRead(ViSession instrID,
                      ViBoolean trace, ViInt16 peakSearch,
                      ViInt16 _VI_FAR traceArray[], ViPReal64 sweepTime,
                      ViChar _VI_FAR sweepUnit[], ViPReal64 MKRNposition,
                      ViPReal64 MKRNlevel);

/*==============================*/
/*   Configurations Fonctions   */
/*==============================*/

/* Configure Frequency*/                    
ViStatus _VI_FUNC AVPA_Set_Freq (ViSession instrID, ViInt16 frequencyType,
                      ViReal64 frequencyValue);
ViStatus _VI_FUNC AVPA_Set_Span (ViSession instrID, ViInt16 spanType,
                      ViReal64 spanValue);
ViStatus _VI_FUNC AVPA_Set_FreqOffset (ViSession instrID, ViBoolean activation,
                      ViReal64 frequencyOffset);
ViStatus _VI_FUNC AVPA_Set_FreqStepSize (ViSession instrID, ViBoolean controlType,
                      ViReal64 frequencyStep);
ViStatus _VI_FUNC AVPA_Set_FreqCounter (ViSession instrID, ViBoolean activation,
                      ViInt16 resolution);
                      
/* Configure Amplitude*/
ViStatus _VI_FUNC AVPA_Set_RefLevel (ViSession instrID, ViReal64 refLevelValue);
ViStatus _VI_FUNC AVPA_Set_RefUnit (ViSession instrID, ViInt16 referenceUnit);

ViStatus _VI_FUNC AVPA_Set_Scale (ViSession instrID, ViInt16 scale);
ViStatus _VI_FUNC AVPA_Set_RefOffset (ViSession instrID, ViBoolean activation,
                      ViReal64 offset);
ViStatus _VI_FUNC AVPA_Set_Att (ViSession instrID, ViBoolean controlType,
                      ViReal64 value);

ViStatus _VI_FUNC AVPA_Set_MinATT (ViSession instrID, ViBoolean controlType,
                      ViReal64 value);
                      
/* Configure Bandwidth*/
ViStatus _VI_FUNC AVPA_Set_RBW (ViSession instrID, ViInt16 RBWSetting);
ViStatus _VI_FUNC AVPA_Set_VBW (ViSession instrID, ViInt16 VBWSetting);
ViStatus _VI_FUNC AVPA_Set_RBW_Span (ViSession instrID, ViBoolean defaultCouple,
                      ViReal64 ratioRBWtoSpan);
ViStatus _VI_FUNC AVPA_Set_RBW_VBW (ViSession instrID, ViBoolean defaultCouple,
                      ViReal64 ratioVBWtoRBW);
ViStatus _VI_FUNC AVPA_Set_All_Auto_Couple (ViSession instrID);

/* Configure Sweep */
ViStatus _VI_FUNC AVPA_Set_SweepTime (ViSession instrID, ViBoolean controlType,
                      ViReal64 sweepTime);
ViStatus _VI_FUNC AVPA_Set_SweepMode (ViSession instrID, ViBoolean sweepMode,
                      ViBoolean windowSweep);
ViStatus _VI_FUNC AVPA_Set_TriggerMode (ViSession instrID, ViInt16 triggerSource,
                      ViBoolean polarityExtTrig, ViInt16 levelVideoTrig);
ViStatus _VI_FUNC AVPA_Set_GatedSweep (ViSession instrID, ViBoolean gatedSweep,
                      ViInt16 gateSource, ViBoolean gateSlope, ViReal64 gatePosition,
                      ViReal64 gateWidth);
                      
/* Configure Trace */
ViStatus _VI_FUNC AVPA_Set_TraceAMode (ViSession instrID, ViInt16 traceAMode,
                      ViInt16 averageCount, ViBoolean averageMode);
ViStatus _VI_FUNC AVPA_Set_TraceBMode (ViSession instrID, ViInt16 traceBMode);
ViStatus _VI_FUNC AVPA_Set_TraceDetector (ViSession instrID, ViInt16 detectorMode);
ViStatus _VI_FUNC AVPA_Set_TracePoints (ViSession instrID, ViBoolean nbrTracePoints);
                      
/* Configure Display &Limit Lines */
ViStatus _VI_FUNC AVPA_Set_LimitLine (ViSession instrID, ViBoolean limitLine,
                      ViString LL_FreqTable, ViString LL_LevelTable);
ViStatus _VI_FUNC AVPA_Activate_LimitLine (ViSession instrID, ViBoolean LL1Status,
                      ViBoolean LL2Status, ViBoolean levelXaxis,
                      ViBoolean levelYaxis);
ViStatus _VI_FUNC AVPA_Move_LimitLine (ViSession instrID, ViBoolean shiftType,
                      ViReal64 shiftValue);
ViStatus _VI_FUNC AVPA_Set_DisplayLine (ViSession instrID, ViBoolean showDL,
                      ViReal64 DLposition);
                      
/* Configure Measurement Window */
ViStatus _VI_FUNC AVPA_Set_MeasurementWindow (ViSession instrID, ViBoolean showHide,
                      ViReal64 position, ViReal64 width);
                      
/* Configure Level Correction */
ViStatus _VI_FUNC AVPA_Set_LevelCorrection (ViSession instrID, ViBoolean LCstatus,
                      ViString LC_FreqTable, ViString LC_LevelTable);

/* Configure Markers Normal & Delta */
ViStatus _VI_FUNC AVPA_Set_MarkerNormal (ViSession instrID, ViReal64 MKRNposition);
ViStatus _VI_FUNC AVPA_Set_MarkerDelta (ViSession instrID, ViReal64 MKRDposition);
ViStatus _VI_FUNC AVPA_Set_MarkerDeltaOptions (ViSession instrID,
                      ViBoolean one1DeltaMarker, ViBoolean MKRDfixed,
                      ViBoolean DisplayMode);
ViStatus _VI_FUNC AVPA_Set_MarkerTrace (ViSession instrID, ViBoolean markerTrace);

/* Configure Multimarkers */

ViStatus _VI_FUNC AVPA_Activate_Multimarkers (ViSession instrID);
ViStatus _VI_FUNC AVPA_Set_Multimarkers (ViSession instrID, ViString MKRMstatus,
                      ViString MKRMpositions);
ViStatus _VI_FUNC AVPA_Set_ActiveMultiMarker (ViSession instrID, ViInt16 MKRMactive);
ViStatus _VI_FUNC AVPA_Delete_Markers (ViSession instrID);
                      
/* Configure Peak Search */
ViStatus _VI_FUNC AVPA_Set_PeakSearchOptions (ViSession instrID,
                      ViBoolean continuousSearch, ViBoolean signalTrack,
                      ViBoolean coupleToWindow, ViInt16 peakRange, ViReal64 deltaY);
                      
/*=============================*/
/*   Action/Status Functions   */
/*=============================*/
ViStatus _VI_FUNC AVPA_Search_Peak (ViSession instrID, ViInt16 searchType);
ViStatus _VI_FUNC AVPA_Execute_PeakTo (ViSession instrID, ViBoolean peakTo);
ViStatus _VI_FUNC AVPA_Set_MarkerDeltaTo (ViSession instrID, ViInt16 markerDeltaTo);

/* Trace Action */
ViStatus _VI_FUNC AVPA_Take_Sweep (ViSession instrID, ViInt16 startSweep);
ViStatus _VI_FUNC AVPA_Execute_TraceMath (ViSession instrID, ViInt16 traceMath);
ViStatus _VI_FUNC AVPA_Control_TraceAverage (ViSession instrID, ViInt16 traceAverage);
                      
/* Configuration Read */
ViStatus _VI_FUNC AVPA_Read_FreqSettings (ViSession instrID,
                      ViInt16 frequencyParameter, ViPReal64 frequencyValue);
ViStatus _VI_FUNC AVPA_Read_FrequencyOffset (ViSession instrID,
					  ViPReal64 value,ViPInt16 status);                      
ViStatus _VI_FUNC AVPA_Read_FreqStepSize (ViSession instrID,
                      ViPReal64 frequencyStepSize, ViPInt16 controlStatus);
ViStatus _VI_FUNC AVPA_Read_FreqCounter (ViSession instrID, ViPReal64 counterValue,
                      ViPInt16 counterStatus);
ViStatus _VI_FUNC AVPA_Read_ReferenceLevel(ViSession instrID,
                      ViPReal64 refLevelValue);
ViStatus _VI_FUNC AVPA_Read_RefLevelOffset (ViSession instrID, ViPReal64 value,
                      ViPInt16 controlType);
ViStatus _VI_FUNC AVPA_Read_RefUnit (ViSession instrID, ViPInt16 referenceUnitNumber,
                      ViChar _VI_FAR referenceUnitName[]);
ViStatus _VI_FUNC AVPA_Read_Scale (ViSession instrID, ViPInt16 status,
                      ViChar _VI_FAR scaleName[]);
ViStatus _VI_FUNC AVPA_Read_Attenuation (ViSession instrID,
                      ViBoolean attenuationtoRead, ViPReal64 valuedB,
                      ViPInt16 controlType);
ViStatus _VI_FUNC AVPA_Read_RBWandVBW (ViSession instrID,
                      ViBoolean bandwidthtoRead, ViPReal64 bandwidthValue,
                      ViPInt16 controlType);
ViStatus _VI_FUNC AVPA_Read_SweepTime (ViSession instrID, ViPReal64 timeValue,
                      ViPInt16 controlType);
ViStatus _VI_FUNC AVPA_Read_SweepMode (ViSession instrID, ViPInt16 sweepModeStatus,
                      ViChar _VI_FAR sweepMode[]);
ViStatus _VI_FUNC AVPA_Read_TriggerMode (ViSession instrID,
                      ViPInt16 triggerModeStatus, ViChar _VI_FAR triggerMode[],
                      ViPInt16 levelVideoTrigger);
ViStatus _VI_FUNC AVPA_Read_GatedSweep (ViSession instrID, ViPInt16 status,
                      ViChar _VI_FAR message[], ViPReal64 gatePosition,
                      ViPReal64 gateWidth, ViPInt16 sourceStatus,
                      ViChar _VI_FAR sourceType[]);
ViStatus _VI_FUNC AVPA_Read_DisplayLine(ViSession instrID, ViPReal64 level,
                      ViPInt16 status, ViChar _VI_FAR message[]);
ViStatus _VI_FUNC AVPA_Read_LimitLineOptions (ViSession instrID, ViBoolean parameter,
                      ViPInt16 levelStatus, ViChar _VI_FAR levelMessage[],
                      ViPReal64 shiftlevel, ViPReal64 shiftTF);
ViStatus _VI_FUNC AVPA_Read_MeasurementWindow (ViSession instrID,
                      ViPReal64 windowPos, ViPReal64 windowWidth, ViPInt16 status,
                      ViChar _VI_FAR message[]);
ViStatus _VI_FUNC AVPA_Read_Peak_Options (ViSession instrID, ViPInt16 contPeakStatus,
                      ViChar _VI_FAR contPeakMessage[], ViPInt16 signalTrackStatus,
                      ViChar _VI_FAR signalTrackMessage[], ViPInt16 coupleStatus,
                      ViChar _VI_FAR coupleMessage[], ViPReal64 deltaY);

/* Status Read */
ViStatus _VI_FUNC AVPA_Read_TraceStatus (ViSession instrID,
                      ViBoolean traceModetoRead, ViPInt16 traceModeStatus,
                      ViChar generalStatusText[]);
ViStatus _VI_FUNC AVPA_Read_DetectionMode (ViSession instrID,
                      ViPInt16 detectorModeStatus, ViChar _VI_FAR detectorMode[]);
ViStatus _VI_FUNC AVPA_Read_TracePoints(ViSession instrID, ViPInt16 nbPoints);                                            
ViStatus _VI_FUNC AVPA_Read_LimitLineStatus (ViSession instrID,
                      ViBoolean limitLinetoCheck, ViPInt16 limitLineStatus,
                      ViChar _VI_FAR limitLineMessage[]);
ViStatus _VI_FUNC AVPA_Read_LLjudgment (ViSession instrID, ViPInt16 result,
                                                           ViChar _VI_FAR message[]);
ViStatus _VI_FUNC AVPA_Read_LevelCorrectionStatus (ViSession instrID,
                      ViPInt16 LCstatus, ViChar _VI_FAR LCmessage[]);
ViStatus _VI_FUNC AVPA_Read_MarkerStatus (ViSession instrID, ViPInt16 markerStatus,
                      ViChar _VI_FAR markerStatusMessage[]);
ViStatus _VI_FUNC AVPA_Read_MarkerDeltaOptions (ViSession instrID,
                      ViPInt16 fixedMarkerStatus, ViChar _VI_FAR fixedMarkerMessage[],
                      ViPInt16 displayModeStatus, ViChar _VI_FAR displayMode[]);
ViStatus _VI_FUNC AVPA_Read_MarkerTraceStatus (ViSession instrID,
                      ViPInt16 markerTraceStatus, ViChar _VI_FAR markerTraceMessage[]);
ViStatus _VI_FUNC AVPA_Read_MultimarkersStatus (ViSession instrID,
                      ViPInt16 multimarkersStatus,
                      ViChar _VI_FAR multimarkersMessage[]);
                      
/*====================*/
/*   Data Functions   */
/*====================*/
/* Trace Function */
ViStatus _VI_FUNC AVPA_Read_TraceData (ViSession instrID, ViBoolean tracetoRead,
                      ViBoolean dataFormat, ViInt16 _VI_FAR traceDataArray[]);

/* Markers Functions */
ViStatus _VI_FUNC AVPA_Read_MarkerNormal (ViSession instrID, ViPReal64 MKRNposition,
                      ViPReal64 MKRNlevel);
ViStatus _VI_FUNC AVPA_Read_MarkerDelta (ViSession instrID, ViPReal64 MKRDposition,
                      ViPReal64 MKRDlevel);
ViStatus _VI_FUNC AVPA_Read_Multimarkers (ViSession instrID,
                      ViReal64 _VI_FAR MKRMpositions[], ViPReal64 MKRDposition,
                      ViReal64 _VI_FAR MKRMlevels[], ViPReal64 markerDeltaLevel);
ViStatus _VI_FUNC AVPA_Read_MultiMarkerActive (ViSession instrID,
                      ViPInt16 multiMarkerActive);
                      
/*=======================*/               
/*   Utility Functions*/
/*=======================*/
ViStatus _VI_FUNC AVPA_Calibration (ViSession instrID, ViInt16 elementtoCalibrate,
                      ViReal64 calibrationLevel, ViBoolean calibrationCorrection,
                      ViBoolean frequencyCorrection);
ViStatus _VI_FUNC AVPA_Display_Label (ViSession instrID, ViBoolean showLabel,
                      ViString label);
ViStatus _VI_FUNC AVPA_Copy_Screen (ViSession instrID, ViInt16 copyMode);
ViStatus _VI_FUNC AVPA_Read_StatusByte (ViSession instrID, ViPInt16 statusByteRead);
ViStatus _VI_FUNC AVPA_Clear_StatusByte (ViSession instrID);

/* Frequency or Time Conversion */
ViStatus _VI_FUNC AVPA_Convert_Frequency (ViSession instrID,
                      ViReal64 inputFrequencyValue, ViPReal64 outputFrequencyValue,
                      ViChar _VI_FAR frequencyUnit[]);
ViStatus _VI_FUNC AVPA_Convert_Time (ViSession instrID, ViReal64 inputTimeValue,
                      ViPReal64 outputTimeValue, ViChar _VI_FAR timeUnit[]);
                      
/* Memory Card Management */
ViStatus _VI_FUNC AVPA_Init_MemoryCard (ViSession instrID, ViBoolean memoryCardDrive);
ViStatus _VI_FUNC AVPA_Select_MemoryDrive (ViSession instrID, ViInt16 memoryDrive);
ViStatus _VI_FUNC AVPA_Copy_MemoryCard (ViSession instrID);
ViStatus _VI_FUNC AVPA_Recall_File (ViSession instrID,
                      ViBoolean recallType, ViInt16 registerName,
                      ViString fileName);
ViStatus _VI_FUNC AVPA_Save_File (ViSession instrID, ViBoolean saveType,
                      ViInt16 registerName, ViString fileName);
ViStatus _VI_FUNC AVPA_Delete_File (ViSession instrID, ViBoolean deleteType,
                      ViInt16 registerName, ViString fileName);

/*other functions*/
ViStatus _VI_FUNC AVPA_WriteInstrData (ViSession instrID, ViString writeBuffer);
ViStatus _VI_FUNC AVPA_readInstrData (ViSession instrID,
                      ViInt32 numberBytesToRead, ViChar _VI_FAR readBuffer[],
                      ViPInt32 numBytesRead);
ViStatus _VI_FUNC AVPA_reset (ViSession instrSession);
ViStatus _VI_FUNC AVPA_selfTest (ViSession instrID, ViPInt16 selfTestResult,
                                                    ViChar _VI_FAR selfTestMessage[]);
ViStatus _VI_FUNC AVPA_errorQuery (ViSession instrID, ViPInt32 errorCode,
                      ViChar _VI_FAR errorMessage[]);
ViStatus _VI_FUNC AVPA_errorMessage (ViSession instrID, ViStatus statusCode,
                      ViChar _VI_FAR message[]);
ViStatus _VI_FUNC AVPA_revisionQuery (ViSession instrID,
                      ViChar _VI_FAR instrumentDriverRevision[],
                      ViChar _VI_FAR firmwareRevision[]);                           
ViStatus _VI_FUNC AVPA_close (ViSession instrID);


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

/*===========================================================================*/
/* END INCLUDE FILE                                                          */
/*===========================================================================*/

#endif
