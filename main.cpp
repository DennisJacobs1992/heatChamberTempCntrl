#include "GUIslice.h"
#include "GUIslice_drv.h"

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    5
gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];

bool m_bQuit = false;

// Button callback
bool CbBtnQuit(void* pvGui,void *pvElem,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (eTouch == GSLC_TOUCH_UP_IN) {
    m_bQuit = true;
  }
  return true;
}


int main( int argc, char* args[] )
{
  gslc_tsElem*  pElem = NULL;

  // Initialize
  gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,NULL,0));
  
  // Build the main page
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPageElem,MAX_ELEM_PG_MAIN);  
  
  // Background flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_GRAY_DK2);

  // Create background box
  pElem = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX,E_PG_MAIN,(gslc_tsRect){10,50,300,150});
  gslc_ElemSetCol(pElem,GSLC_COL_WHITE,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // Create Quit button with image label and callback
  pElem = gslc_ElemCreateBtnImg(&m_gui,E_ELEM_BTN_QUIT,E_PG_MAIN,(gslc_tsRect){258,70,32,32},
          gslc_GetImageFromFile("btn-exit.bmp",GSLC_IMGREF_FMT_BMP16),
          gslc_GetImageFromFile("btn-exit-sel.bmp",GSLC_IMGREF_FMT_BMP16),
          &CbBtnQuit);

  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  // Main event loop
  m_bQuit = false;
  while (!m_bQuit) {
    // Update GUI
    gslc_Update(&m_gui);
  }

  // Close down display
  gslc_Quit(&m_gui);
  return 0;
}
