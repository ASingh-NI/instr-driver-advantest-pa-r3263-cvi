/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 1999. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  BACKGROUND                      1

#define  INIT                            2
#define  INIT_OK                         2       /* callback function: initialize */
#define  INIT_CANCEL                     3       /* callback function: quitPanel */
#define  INIT_GPIBaddress                4       /* callback function: GPIBHelp */
#define  INIT_RESET                      5       /* callback function: resetHelp */

#define  PANEL                           3
#define  PANEL_RUN                       2       /* callback function: trace */
#define  PANEL_QUIT                      3       /* callback function: shutDown */
#define  PANEL_SPAN                      4       /* callback function: spanHelp */
#define  PANEL_PEAK                      5       /* callback function: peakHelp */
#define  PANEL_CENTER                    6       /* callback function: centerHelp */
#define  PANEL_TRACE                     7       /* callback function: traceHelp */
#define  PANEL_SWP_V                     8
#define  PANEL_GRAPH                     9       /* callback function: waveHelp */
#define  PANEL_TXTFOF                    10
#define  PANEL_TEXTUNCALL                11
#define  PANEL_TEXTLOF                   12
#define  PANEL_TEXTFOF                   13
#define  PANEL_SWP_U                     14
#define  PANEL_TIMER                     15      /* callback function: timerControl */


     /* Menu Bars, Menus, and Menu Items: */

#define  backMenu                        1
#define  backMenu_ABOUTBACK              2       /* callback function: backHelp */

#define  Initialize                      2
#define  Initialize_ABOUTINIT            2       /* callback function: initHelp */

#define  panelMenu                       3
#define  panelMenu_ABOUTPANEL            2       /* callback function: panelHelp */


     /* Callback Prototypes: */ 

void CVICALLBACK backHelp(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK centerHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GPIBHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK initHelp(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK initialize(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK panelHelp(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK peakHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK quitPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK resetHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK shutDown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK spanHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerControl(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK trace(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK traceHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK waveHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
