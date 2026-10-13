#include <formatio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cvirte.h>     /* Needed if linking in external compiler; harmless otherwise */
#include "AVPAu.h"
#include "AVPA.h"

static int initHandle;
static int panelHandle;
static int backgroundHandle;

static ViSession instrID;
static ViStatus uirErr;
static int uirCode, ini=0;
static ViInt16 traceName=1;
static ViInt16 peakSearch=0;
static ViReal64 center, span;

void abortApplication(void);  

void main ()
{
    
    backgroundHandle = LoadPanel (0, "AVPAu.uir", BACKGROUND);
    panelHandle = LoadPanel (backgroundHandle,"AVPAu.uir",PANEL);
    initHandle = LoadPanel (backgroundHandle,"AVPAu.uir",INIT);
       
    DisplayPanel (backgroundHandle);
    DisplayPanel (initHandle); 
    uirCode = RunUserInterface ();
    
 }


/*****************************************************************************/
int initialize (int panel, int control, int event,void *callbackData,
                int eventData1, int eventData2)
{
 
    ViBoolean reset;
    ViChar resourceName[20]={"\0"};
    SuspendTimerCallbacks();
    
    switch (event) {
    
        case EVENT_COMMIT:
        
            GetCtrlVal (initHandle,INIT_GPIBaddress,resourceName);
            GetCtrlVal (initHandle,INIT_RESET,&reset);
            if ((uirErr = AVPA_init (resourceName, VI_ON, reset, &instrID)) < 0)
                abortApplication();
             
            DisplayPanel(panelHandle);   
          
            if ((uirErr = AVPA_Read_FreqSettings(instrID,0,&center)) < 0)
                abortApplication();
            SetCtrlVal(panelHandle,PANEL_CENTER,center);
            if ((uirErr = AVPA_Read_FreqSettings(instrID,1,&span)) < 0)
                abortApplication();
            SetCtrlVal(panelHandle,PANEL_SPAN,span);
          
            ini=1;
            break;  
        
        case EVENT_RIGHT_CLICK:
        
            MessagePopup ("Initialize Help","This control initializes the instrument and calls example function");
            break;
    }
    
    return 0;
}

/*****************************************************************************/
int shutDown (int panel, int control, int event,void *callbackData,
              int eventData1, int eventData2)
{
    
    int close;
    
    switch (event) {
    
        case EVENT_COMMIT:
            if ((close=ConfirmPopup("Confirm","Do you want really to quit?"))==1) {
                ini=0;
                if ((uirErr=AVPA_close (instrID)) < 0)
                    abortApplication();
                QuitUserInterface (uirCode);
            }
            break;
          
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Quit Help","This control closes the instrument and returns control to the user");
            break;
    }
     
    return 0;
}

/*****************************************************************************/
void abortApplication()
{
    ViChar errorMessage[256], errBuffer[256];

    SetWaitCursor (0);
    AVPA_errorMessage (VI_NULL, uirErr, errorMessage);
    Fmt (errBuffer,"%s<The following error caused the application to fail:\n\n%s\n\n"
        "Attempting to Close Instrument Driver and Exit Application", 
        errorMessage);
    MessagePopup("ERROR!, Application Failed", errBuffer);
    
    if ((uirErr = AVPA_close (instrID)) < 0) {
        AVPA_errorMessage (VI_NULL, uirErr, errorMessage);
        Fmt (errBuffer, "%s<Application failed to close the Instrument Driver.\n\n"
            "%s\n\nExiting Application.", errorMessage);
        MessagePopup("ERROR!",errBuffer);
    }
    else 
        MessagePopup("Application Aborted","Instrument Driver Closed, Exiting Application.");
        
    QuitUserInterface (uirCode);
    exit (-1);
}        
          
/*****************************************************************************/
int quitPanel (int panel, int control, int event,
               void *callbackData, int eventData1, int eventData2)
        
{
    switch (event) {
    
        case EVENT_COMMIT:
            if ((ConfirmPopup ("Exit Application", "Would you like to exit this "
                   "application?")) == 1) {
                QuitUserInterface (uirCode);
            }
            break;
          
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Cancel Help","This control closes the instrument and returns controls to the user");
            break;
    }
     
    return 0;
}

/*****************************************************************************/
int trace (int panel, int control, int event,void *callbackData,
           int eventData1, int eventData2)
{
    
    ViReal64 sweepTime,MKRNposition,MKRNlevel,start;
    int i,waveform[1001];
    ViInt16 x,traceData[1001];
    ViChar sweepUnit[10];
    
    if (ini!=0) {
    
        ResetTimer (panelHandle,PANEL_TIMER);
        ResumeTimerCallbacks();
    
        DeleteGraphPlot (panelHandle, PANEL_GRAPH, -1, VAL_DELAYED_DRAW);
    
        GetCtrlVal (panelHandle,PANEL_CENTER,&center);
        if ((uirErr=AVPA_Set_Freq(instrID,0,center)) < 0)
            abortApplication();
       
        GetCtrlVal (panelHandle,PANEL_SPAN,&span);
        if ((uirErr=AVPA_Set_Span(instrID,0,span)) < 0)
            abortApplication();
    
        GetCtrlVal (panelHandle,PANEL_PEAK,&peakSearch);
        GetCtrlVal (panelHandle,PANEL_TRACE,&traceName);
       
        if ((uirErr = AVPA_configRead(instrID,traceName,peakSearch,traceData, &sweepTime,
                                  sweepUnit,&MKRNposition,&MKRNlevel)) < 0)
            abortApplication();
        
        for (i=0;i<1001;i++) {
            waveform[i]=traceData[i];
        }
     
        PlotY (panelHandle, PANEL_GRAPH, waveform, 1001, VAL_INTEGER,VAL_THIN_LINE,
               VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_YELLOW);
    
        if (traceName==1) {
            start = center-0.5*span; 
            x = ceil(1 + (1001-1)*(MKRNposition-start)/span);
            PlotPoint(panelHandle,PANEL_GRAPH,x,waveform[x-1],VAL_SOLID_DIAMOND,VAL_RED);       
        }
    
        SetCtrlVal(panelHandle,PANEL_SWP_V,sweepTime);
        SetCtrlVal(panelHandle,PANEL_SWP_U,sweepUnit);
       
        SuspendTimerCallbacks();
       
    }
    
    return 0;
}

/*****************************************************************************/
int timerControl (int panel, int control, int event,void *callbackData,
                  int eventData1, int eventData2)
{
 
    MessagePopup("Timer Control","application is working");
    return 0;
}

/*****************************************************************************/
int GPIBHelp (int panel, int control, int event,void *callbackData,
              int eventData1, int eventData2)
{
 
    switch (event) {
    
        case EVENT_RIGHT_CLICK:
            MessagePopup ("GPIB Address Help","This control specifies the interface and address of the device \n"
            "that is to be initialized (Instrument Descriptor). The exact\n"
            "grammar to be used in this control is shown in the note below.\n\n"
            "Default Value:  \"GPIB::1\"\n\n"
            "Notes: \n\n"
            "(1) Based on the Instrument Descriptor, this operation\n"
            "establishes a communication session with a device. The grammar\n"
            "for the Instrument Descriptor is shown below. Optional\n"
            "parameters are shown in square brackets ([]).\n\n"
            "(2) Interface   Grammar\n"
            "------------------------------------------------------\n"
            "GPIB        GPIB[board]::primary address[::secondary address]\n"
            "            [::INSTR]\n\n"
            "The GPIB keyword is used with GPIB instruments.\n"
            "The default value for optional parameters are shown below.\n"
            "Optional Parameter          Default Value\n"
            "-----------------------------------------\n"
            "board                       0\n"
            "secondary address           none - 31");
            break;
    }
     
    return 0;
}

/*****************************************************************************/
int resetHelp (int panel, int control, int event,void *callbackData,
               int eventData1, int eventData2)
{
 
    switch (event) {
        
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Reset Help","This control specifies if the instrument is to be reset to its\n"
            "power-on settings during the initialization procedure.\n\n"
            "Valid Range:\n"
            "VI_OFF (0) - Don't Reset\n"
            "VI_ON  (1) - Reset Device (Default Value)\n\n"
            "Note:If you do not want the instrument reset.\n"
            "Set this control to \"Don't Reset\" while initializing the instrument.");
            break;
    }
     
    return 0;
}

/*****************************************************************************/
int centerHelp (int panel, int control, int event,void *callbackData,
                int eventData1, int eventData2)
{
 
    switch (event) {
    
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Center Frequency Help","This control sets the center frequency\n\n"
            "Valid Range :\n"
            "9 kHz to 3 GHz for the R3263 and R3463\n"
            "9 kHz to 9 GHZ for the R3465\n"
            "9 kHz to 26.5 GHz for the R3272\n\n"
            "Default Value: 1 GHz\n\n"
            "Note : the frequency is always expressed in Hz.");
            break;
    }
    
    return 0;
}

/*****************************************************************************/
int spanHelp (int panel, int control, int event,void *callbackData,
              int eventData1, int eventData2)
{
 
    switch (event) {
        
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Span Frequency Help","This control specifies the frequency span\n\n"
            "Valid Range :\n"
            "0 to 3 GHz for the R3263 and R3463\n"
            "0 to 8 GHz for the R3465\n"
            "0 to 26.5 GHz for the R3272\n\n"
            "Default Value: 100 MHz\n\n"
            "Note : The frequency span is always expressed in Hz.");
            break;
    }
    
    return 0;
}
 
/*****************************************************************************/
int tracePointsHelp (int panel, int control, int event,void *callbackData,
                     int eventData1, int eventData2)
{
 
    switch (event) {
    
        case EVENT_RIGHT_CLICK:
            MessagePopup("Trace Points Help","selects the number of points in the screen trace.\n\n"
            "Valid Range:\n"
            "VI_ON - 501 points\n"
            "VI_OFF - 1001 points (Default Value)");
            break;
        
    }
     
    return 0;
} 

/*****************************************************************************/
int traceHelp (int panel, int control, int event,void *callbackData,
               int eventData1, int eventData2)
{
 
    switch (event) {
    
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Trace Help","selects the trace to display on screen.\n\n"
            "Valid Range :\n"
            "VI_ON - Trace A (Default)\n"
            "VI_OFF - Trace B");
            break;
    }
     
    return 0;
}
 
/*****************************************************************************/ 
int peakHelp (int panel, int control, int event,void *callbackData,
              int eventData1, int eventData2)
{
 
    switch (event) {
        
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Peak Search Help","selects the peak marker search desired\n\n"
            "Valid Range :\n"
            "0 - peak search (Default)\n"
            "1 - next peak\n"
            "2 - next peak left\n"
            "3 - next peak right\n"
            "4 - min search\n"
            "5 - next minimum");
            break;
        
    }
     
    return 0;
}
 
/*****************************************************************************/ 
int waveHelp (int panel, int control, int event,void *callbackData,
              int eventData1, int eventData2)
{
 
    switch (event) {
        
        case EVENT_RIGHT_CLICK:
            MessagePopup ("Graph Help","This control plots the waveform read back from the instument");
            break;
        
    }
     
    return 0;
    
}
 
/*****************************************************************************/
void initHelp (int menuBar, int menuItem, void *callbackData,int panel)
{
   
    MessagePopup ("Instrument Help","This getting started example supports the R3263, R3463, R3465 and R3272 spectrum analyzers \n\n"
    "This panel supports the initialization of the instrument.\n\n"
    "Selecting SEND opens a session, performs an instrument identification query and reset or not the instrument.\n"
    "Afterwards, the action panel appears allowing the execution of the application function.\n\n"
    "Selecting CLOSE returns the control to the user.");
     
}    


/*****************************************************************************/
void backHelp (int menuBar, int menuItem, void *callbackData,int panel)
{
 
    MessagePopup ("Instrument Help","This getting started example supports the R3263, R3463, R3465 and R3272 spectrum analyzers \n\n"
    "After initializing the instrument, the application function can be executed\n\n"
    "This function configures the center frequency and the span of the analyzer, displays the trace A or B,\n"
    "searchs and displays the peak selected on the trace and returns the sweep time value.\n\n"
    "Selecting QUIT causes the instrument to be closed and control returned to the user.");
      
}

/*****************************************************************************/
void panelHelp (int menuBar, int menuItem, void *callbackData,int panel) 
{
 
     MessagePopup ("Instrument Help","This getting started example supports the R3263, R3463, R3465 and R3272 spectrum analyzers \n\n"
     "After initializing the instrument, the application function can be executed\n\n"
     "This function configures the center frequency and the span of the analyzer, displays the trace A or B,\n"
     "searchs and displays the peak selected on the trace and returns the sweep time value.\n\n"
     "Selecting QUIT causes the instrument to be closed and control returned to the user.");
      
} 


/*****************************************************************************/
/*=== END UIR SOURCE CODE ===================================================*/
/*****************************************************************************/
