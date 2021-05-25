//
// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 02: Accept touch input, text button
//

#include "GUIslice.h"
#include "GUIslice_drv.h"


// Defines for resources
#define FONT1 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum {  E_PG_MAIN};
enum {  E_ELEM_BOX,
        E_ELEM_BTN_LIGHT,       
        E_ELEM_BTN_MinTempPlus,
        E_ELEM_BTN_MinTempMinus,
        E_ELEM_BTN_MaxTempPlus,
        E_ELEM_BTN_MaxTempMinus,
        E_ELEM_BTN_DeafaultForPLA,
        E_ELEM_BTN_DeafaultForABS,
        E_ELEM_BTN_TimePlus,
        E_ELEM_BTN_TimeMinus,
        E_ELEM_BTN_StartStop,
        E_ELEM_DATATIMEH,
        E_ELEM_DATATIMEM,
        E_ELEM_DATATEMPMIN,
        E_ELEM_DATATEMPMAX,
        E_ELEM_DATATEMPREAD};
enum {  E_FONT_BTN,
        E_FONT_BTN_LIGHT,
        E_FONT_TXT,
        E_FONT_TXTBIG,
        E_FONT_TXTSMALL,
        MAX_FONT};

bool    m_bQuit = false;
char StartStop[5]= "Start";

int     dataTimeDurationH = 0;
int     dataTimeDurationM = 0;
int     dataTempMax = 30;
int     dataTempMin = 20;
int     dataTempRead = 78;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    27

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

#define MAX_STR             100

#define MAX_TEMP            70
#define MIN_TEMP            20

// Configure environment variables suitable for display
// - These may need modification to match your system
//   environment and display type
// - Defaults for GSLC_DEV_FB and GSLC_DEV_TOUCH are in GUIslice_config.h
// - Note that the environment variable settings can
//   also be set directly within the shell via export
//   (or init script).
//   - eg. export TSLIB_FBDEVICE=/dev/fb1
void UserInitEnv()
{
#if defined(DRV_DISP_SDL1) || defined(DRV_DISP_SDL2)
  setenv((char*)"FRAMEBUFFER",GSLC_DEV_FB,1);
  setenv((char*)"SDL_FBDEV",GSLC_DEV_FB,1);
  setenv((char*)"SDL_VIDEODRIVER",GSLC_DEV_VID_DRV,1);
#endif

#if defined(DRV_TOUCH_TSLIB)
  setenv((char*)"TSLIB_FBDEVICE",GSLC_DEV_FB,1);
  setenv((char*)"TSLIB_TSDEVICE",GSLC_DEV_TOUCH,1);
  setenv((char*)"TSLIB_CALIBFILE",(char*)"/etc/pointercal",1);
  setenv((char*)"TSLIB_CONFFILE",(char*)"/etc/ts.conf",1);
  setenv((char*)"TSLIB_PLUGINDIR",(char*)"/usr/local/lib/ts",1);
#endif
}

// Define debug message function
static int16_t DebugOut(char ch) { fputc(ch,stderr); return 0; }

// Button callbacks

bool CbBtnLight(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

bool CbBtnMinTempPlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMin++;
  if((dataTempMax-5)<=dataTempMin){
    dataTempMin = dataTempMax - 5;
  }
  return true;
}

bool CbBtnMinTempMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMin--;
  if(dataTempMin <= MAX_TEMP){
    dataTempMin = MIN_TEMP;
  }
  return true;
}

bool CbBtnMaxTempPlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMax++;
  if(dataTempMin >= MAX_TEMP){
    dataTempMin = MAX_TEMP;
  }
  return true;
  return true;
}

bool CbBtnMaxTempMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMax--;
  if((dataTempMin+5)>=dataTempMax){
    dataTempMax = dataTempMin + 5;
  }
  return true;
}

bool CbBtnLoadDeafaultForPLA(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMax = 40;
  dataTempMin = 50;
  return true;
}

bool CbBtnLoadDeafaultForABS(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTempMax = 70;
  dataTempMin = 60;
  return true;
}

bool CbBtnTimePlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  dataTimeDurationM = dataTimeDurationM + 15;
  if(dataTimeDurationM>=60){
    dataTimeDurationM = 0;
    dataTimeDurationH++;
  }
  return true;
}

bool CbBtnTimeMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if(dataTimeDurationM=0){
    dataTimeDurationM = 45;
    dataTimeDurationH--;
  }
  else{
    dataTimeDurationM = dataTimeDurationM-15;
  }
  return true;
}

bool CbBtnStartStop(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

int main( int argc, char* args[] )
{
  bool              bOk = true;
  gslc_tsElemRef*   pElemRef = NULL;
  char              acTxt[100];

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  bOk = gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,20);
  if (!bOk) { printf("ERROR: gslc_FontSet() failed\n"); exit(1); }

  bOk = gslc_FontSet(&m_gui,E_FONT_TXT,GSLC_FONTREF_FNAME,FONT1,30);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }

  bOk = gslc_FontSet(&m_gui,E_FONT_TXTBIG,GSLC_FONTREF_FNAME,FONT1,100);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }

  bOk = gslc_FontSet(&m_gui,E_FONT_TXTSMALL,GSLC_FONTREF_FNAME,FONT1,15);
  if (!bOk) { fprintf(stderr,"ERROR: FontSet failed\n"); exit(1); }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){0,0,480,320});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create buttons
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_LIGHT,E_PG_MAIN,
    (gslc_tsRect){10,10,100,50},"Light",0,E_FONT_BTN,&CbBtnLight);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_TimePlus,E_PG_MAIN,
    (gslc_tsRect){370,80,100,50},"+ TIME",0,E_FONT_BTN,&CbBtnTimePlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_TimeMinus,E_PG_MAIN,
    (gslc_tsRect){180,80,100,50},"- TIME",0,E_FONT_BTN,&CbBtnTimeMinus);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MaxTempPlus,E_PG_MAIN,
    (gslc_tsRect){370,200,100,50},"+ MAX",0,E_FONT_BTN,&CbBtnMaxTempPlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MaxTempMinus,E_PG_MAIN,
    (gslc_tsRect){180,200,100,50},"- MAX",0,E_FONT_BTN,&CbBtnMaxTempPlus);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MinTempPlus,E_PG_MAIN,
    (gslc_tsRect){370,260,100,50},"+ MIN",0,E_FONT_BTN,&CbBtnMinTempPlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MinTempMinus,E_PG_MAIN,
    (gslc_tsRect){180,260,100,50},"- MIN",0,E_FONT_BTN,&CbBtnMinTempMinus);    

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForPLA,E_PG_MAIN,
    (gslc_tsRect){215,140,100,50},"PLA",0,E_FONT_BTN,&CbBtnLoadDeafaultForPLA);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForABS,E_PG_MAIN,
    (gslc_tsRect){335,140,100,50},"ABS",0,E_FONT_BTN,&CbBtnLoadDeafaultForABS);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_StartStop,E_PG_MAIN,
    (gslc_tsRect){180,10,290,50},StartStop,0,E_FONT_BTN,&CbBtnStartStop);

  // Create texts
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATIMEH,E_PG_MAIN,(gslc_tsRect){282,80,30,50},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){317,80,5,50},
    ":",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATIMEM,E_PG_MAIN,(gslc_tsRect){331,80,30,50},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATEMPMAX,E_PG_MAIN,(gslc_tsRect){288,200,40,50},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATEMPMIN,E_PG_MAIN,(gslc_tsRect){288,260,40,50},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){324,200,25,50},
    "~C",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){324,260,25,50},
    "~C",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATEMPREAD,E_PG_MAIN,(gslc_tsRect){5,225,120,100},
    "",0,E_FONT_TXTBIG);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){125,275,25,50},
    "~C",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,130,25,20},
    "Sensor 1:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){110,130,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,150,25,20},
    "Sensor 2:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){110,150,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,170,25,20},
    "Sensor 3:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){110,170,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,205,25,20},
    "Chamber average:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);


  gslc_tsElemRef*  pElemDataTimeH     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATIMEH);
  gslc_tsElemRef*  pElemDataTimeM     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATIMEM);
  gslc_tsElemRef*  pElemDataTempMax   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATEMPMAX);
  gslc_tsElemRef*  pElemDataTempMin   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATEMPMIN);
  gslc_tsElemRef*  pElemDataTempRead  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATEMPREAD);

  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {
    
    //print time
    snprintf(acTxt,MAX_STR,"%02d",dataTimeDurationH);
    gslc_ElemSetTxtStr(&m_gui,pElemDataTimeH,acTxt);
    snprintf(acTxt,MAX_STR,"%02d",dataTimeDurationM);
    gslc_ElemSetTxtStr(&m_gui,pElemDataTimeM,acTxt);

    //print temp settings
    snprintf(acTxt,MAX_STR,"%02d",dataTempMax);
    gslc_ElemSetTxtStr(&m_gui,pElemDataTempMax,acTxt);
    snprintf(acTxt,MAX_STR,"%02d",dataTempMin);
    gslc_ElemSetTxtStr(&m_gui,pElemDataTempMin,acTxt);
    
    //print chamber temp
    snprintf(acTxt,MAX_STR,"%02d",dataTempRead);
    gslc_ElemSetTxtStr(&m_gui,pElemDataTempRead,acTxt);
    
    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}