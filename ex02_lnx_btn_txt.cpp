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
enum {E_PG_MAIN};
enum {  E_ELEM_BOX,
        E_ELEM_BTN_QUIT,
        E_ELEM_BTN_LIGHT};       
        /* 
        E_ELEM_BTN_MinTempPlus,
        E_ELEM_BTN_MinTempMinus,
        E_ELEM_BTN_MaxTempPlus,
        E_ELEM_BTN_MaxTempMinus,
        E_ELEM_BTN_DeafaultForPLA,
        E_ELEM_BTN_DeafaultForABS};
        */
enum {  E_FONT_BTN,
        E_FONT_BTN_LIGHT,
        MAX_FONT};

bool    m_bQuit = false;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    3

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

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
bool CbBtnQuit(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}

bool CbBtnLight(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}
/*
bool CbBtnMinTempPlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

bool CbBtnMinTempMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

bool CbBtnMaxTempPlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

bool CbBtnMaxTempMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

bool CbBtnLoadDeafaultForPLA(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}

bool CbBtnLoadDeafaultForABS(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  return true;
}
*/

int main( int argc, char* args[] )
{
  bool              bOk = true;
  gslc_tsElemRef*   pElemRef = NULL;

  // -----------------------------------
  // Initialize
  gslc_InitDebug(&DebugOut);
  UserInitEnv();
  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { exit(1); }

  // Load Fonts
  bOk = gslc_FontSet(&m_gui,E_FONT_BTN,GSLC_FONTREF_FNAME,FONT1,12);
  if (!bOk) { printf("ERROR: gslc_FontSet() failed\n"); exit(1); }

  // -----------------------------------
  // Create page elements
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN,m_asPageElemRef,MAX_ELEM_PG_MAIN);

  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){0,0,480,320});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create buttons
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,
    (gslc_tsRect){320,100,80,40},"Quit",0,E_FONT_BTN,&CbBtnQuit);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_LIGHT,E_PG_MAIN,
    (gslc_tsRect){20,20,80,40},"Ligh",0,E_FONT_BTN,&CbBtnLight);
/*
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MaxTempPlus,E_PG_MAIN,
    (gslc_tsRect){360,20,100,50},"+ MAX",0,E_FONT_BTN,&CbBtnMaxTempPlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MaxTempMinus,E_PG_MAIN,
    (gslc_tsRect){360,90,100,50},"- MAX",0,E_FONT_BTN,&CbBtnMaxTempPlus);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MinTempPlus,E_PG_MAIN,
    (gslc_tsRect){360,160,100,50},"+ MIN",0,E_FONT_BTN,&CbBtnMinTempPlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MinTempMinus,E_PG_MAIN,
    (gslc_tsRect){360,230,100,50},"- MIN",0,E_FONT_BTN,&CbBtnMinTempMinus);    

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForPLA,E_PG_MAIN,
    (gslc_tsRect){20,90,100,50},"PLA",0,E_FONT_BTN,&CbBtnLoadDeafaultForPLA);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForABS,E_PG_MAIN,
    (gslc_tsRect){20,160,100,50},"ABS",0,E_FONT_BTN,&CbBtnLoadDeafaultForABS);
*/
  // -----------------------------------
  // Start display

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // -----------------------------------
  // Main event loop

  m_bQuit = false;
  while (!m_bQuit) {

    // Periodically call GUIslice update function
    gslc_Update(&m_gui);

  } // bQuit

  // -----------------------------------
  // Close down display

  gslc_Quit(&m_gui);

  return 0;
}