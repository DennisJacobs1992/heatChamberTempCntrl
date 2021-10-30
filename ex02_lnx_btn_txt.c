// GUIslice Library Examples
// - Calvin Hass
// - https://www.impulseadventure.com/elec/guislice-gui.html
// - https://github.com/ImpulseAdventure/GUIslice
// - Example 02: Accept touch input, text button
// change history
// 17/10/2021: - Added quit function for light button.
//             - Added heating function.
//             - Added preheating function.

#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
#include "GUIslice.h"
#include "GUIslice_drv.h"
#include "sensor.h"

// Defines for resources
#define FONT1 "/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"

// Enumerations for pages, elements, fonts, images
enum {  E_PG_MAIN};
enum {  E_ELEM_BOX,
        E_ELEM_BTN_LIGHT,       
        E_ELEM_BTN_MaxTempPlus,
        E_ELEM_BTN_MaxTempMinus,
        E_ELEM_BTN_DeafaultForPLA,
        E_ELEM_BTN_DeafaultForABS,
        E_ELEM_BTN_TimeMinus,
        E_ELEM_BTN_TimePlus,
        E_ELEM_BTN_StartStop,
        E_ELEM_DATATIMEH,
        E_ELEM_DATATIMEM,
        E_ELEM_DATATEMPMAX,
        E_ELEM_DATATEMPREAD,
        E_ELEM_SensorData1,
        E_ELEM_SensorData2,
        E_ELEM_SensorData3,
        E_ELEM_PreHeat};
enum {  E_FONT_BTN,
        E_FONT_BTN_LIGHT,
        E_FONT_TXT,
        E_FONT_TXTBIG,
        E_FONT_TXTSMALL,
        MAX_FONT};

// OneWire variables for reading the devices. Used for the temperature sensors
bool m_bQuit              = false;
char **sensorNames;
char StartStop[5]         = "Start";
int dataTimeDurationH     = 0;
int dataTimeDurationM     = 0;
int dataTempMax           = 30;
int dataTempRead          = 78;
int sensorNamesCount;
float temperature[3];
float dataTempSensor1     = 0;
float dataTempSensor2     = 0;
float dataTempSensor3     = 0;
long heaterDutyCycle      = 0;

// config wiringpi
const int pinLed          = 24;
const int pinFanInternal  = 23;
const int pinFanOut       = 21;
const int pinHeater       = 22;
const int pinPrinter      = 25;

// status variabes
bool disableInput     = 0;
bool lightStatus      = 0;
char heaterStatus     = 0;
bool startStopStatus  = 0;
bool preheatStatus    = 0;
char buttonActive     = 0;
bool quitStatus;

//set clock timers
struct tm minutechecker;
int prevMin = 70;
int prevRoutine = 70;
clock_t quitTime;
clock_t heaterActiveTime;
clock_t buttonActiveTime;

// Instantiate the GUI
#define MAX_PAGE            1
#define MAX_ELEM_PG_MAIN    27
#define MAX_STR             100
#define MAX_OVER_TEMP       1
#define MIN_TEMP            20
#define MAX_TEMP            70

gslc_tsGui                  m_gui;
gslc_tsDriver               m_drv;
gslc_tsFont                 m_asFont[MAX_FONT];
gslc_tsPage                 m_asPage[MAX_PAGE];
gslc_tsElem                 m_asPageElem[MAX_ELEM_PG_MAIN];
gslc_tsElemRef              m_asPageElemRef[MAX_ELEM_PG_MAIN];

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
  if (buttonActive != 3){
    buttonActive = 3;
    if (lightStatus == 0){
      digitalWrite(pinLed, LOW);
      lightStatus = 1;
    }
    else{
      digitalWrite(pinLed, HIGH);
      lightStatus = 0;
    }
  }
  else{
    //check if button was not pressed within 1s to reset the timer
    if ((clock()-buttonActiveTime) > CLOCKS_PER_SEC){
      buttonActive = 0;
    }
  }
  return true;
}

bool CbBtnMaxTempPlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (disableInput == 0){
    if (buttonActive != 4){
      buttonActive = 4;
      buttonActiveTime = clock();
      dataTempMax++;
      if(dataTempMax >= MAX_TEMP){
        dataTempMax = MAX_TEMP;
      }
    }
    else{
      //check if button was not pressed within 300ms to reset the timer
      if ((clock()-buttonActiveTime) > (300*(CLOCKS_PER_SEC/1000))){
        buttonActive = 0;
      }
    }
  }
  return true;
}

bool CbBtnMaxTempMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  digitalWrite(pinLed, HIGH);
  digitalWrite(pinFanInternal, HIGH);
  digitalWrite(pinFanOut, HIGH);
  digitalWrite(pinHeater, HIGH);
  digitalWrite(pinPrinter, HIGH);
  m_bQuit = true; //this line should be moved
  if (disableInput == 0){
    if (buttonActive != 5){
      buttonActive = 5;
      buttonActiveTime = clock();
      dataTempMax--;
      if(dataTempMax >= MIN_TEMP){
        dataTempMax = MIN_TEMP;
      }
    }
    else{
      //check if button was not pressed within 300ms to reset the timer
      if ((clock()-buttonActiveTime) > (300*(CLOCKS_PER_SEC/1000))){
        buttonActive = 0;
      }
    }
  }
  return true;
}

bool CbBtnLoadDeafaultForPLA(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (disableInput == 0){
    dataTempMax = 35;
  }
  return true;
}

bool CbBtnLoadDeafaultForABS(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (disableInput == 0){
    dataTempMax = 65;
  }
  return true;
}

bool CbBtnTimePlus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (buttonActive != 1){
    buttonActive = 1;
    buttonActiveTime = clock();
    if(dataTimeDurationM==55 && dataTimeDurationH<99){
      dataTimeDurationM = 0;
      dataTimeDurationH++;
    }
    else if(dataTimeDurationM<55 && dataTimeDurationH<99){
      dataTimeDurationM += 5;
    }
  }
  else{
    //check if button was not pressed within 300ms to reset the timer
    if ((clock()-buttonActiveTime) > (300*(CLOCKS_PER_SEC/1000))){
      buttonActive = 0;
    }
  }
  return true;
}

bool CbBtnTimeMinus(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (buttonActive != 2){
    buttonActive = 2;
    buttonActiveTime = clock();
    if(dataTimeDurationM == 0 && dataTimeDurationH>0){
      dataTimeDurationM = 55;
      dataTimeDurationH --;
    }
    else if (dataTimeDurationM > 0){
      dataTimeDurationM -= 5;
    }
  }
  else{
    //check if button was not pressed within 300ms to reset the timer
    if ((clock()-buttonActiveTime) > (300*(CLOCKS_PER_SEC/1000))){
      buttonActive = 0;
    }
  }
  return true;
}

bool CbBtnStartStop(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  if (startStopStatus == 0){
    startStopStatus = 1;
  }
  else{
    startStopStatus = 0;
  }
  return true;
}

bool CbBtnPreHeat (void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY){
  if (buttonActive != 6){
    buttonActive = 6;
    buttonActiveTime = clock();
    if (preheatStatus == 1){
      preheatStatus = 0;
    }
    else{
      preheatStatus = 1;
    }
  }
  else{
    //check if button was not pressed within 1s to reset the timer
    if ((clock()-buttonActiveTime) > CLOCKS_PER_SEC){
      buttonActive = 0;
    }
  }
  return true;
}

void setHeater(){
  if (heaterDutyCycle != 0){
    digitalWrite(pinFanInternal, LOW); // enable fans
    switch (heaterStatus)
    {
      case 0:
        heaterActiveTime = clock();
        heaterStatus = 1;
        digitalWrite(pinHeater, LOW); // enable heater
        break;
      case 1:
        if(((clock()-heaterActiveTime)/CLOCKS_PER_SEC) < heaterDutyCycle){
          heaterStatus = 2;
          digitalWrite(pinHeater, HIGH); // disable heater
        }
        break;
      case 2:
        if(((clock()-heaterActiveTime)/CLOCKS_PER_SEC) < 100){
          heaterStatus = 0;          // set heater status back to 0 ready for new cycle
        }
        break;
      default:
        heaterStatus = 0;
        digitalWrite(pinHeater, HIGH); // turn heater off
        break;
    }
  }
  else{
    digitalWrite(pinHeater, HIGH); // turn heater off
  }
  return;
}

void regulateHeat(){
  if (dataTempRead < dataTempMax-5){
   digitalWrite(pinHeater, LOW);
  }
  else if (dataTempRead < dataTempMax-3){
    heaterDutyCycle = 50;
  }
  else if (dataTempRead < dataTempMax-2){
    heaterDutyCycle = 30;
  }
  else if (dataTempRead < dataTempMax-1){
    heaterDutyCycle = 10;
  }
  else if (dataTempRead < dataTempMax+1){
    heaterDutyCycle = 5;
  }
  else if (dataTempRead > dataTempMax+1){
    heaterDutyCycle = 0;
  }
  setHeater();
  return;
}

void executeRoutineTasks(){
  SensorList *sensorList = GetSensors(sensorNames, sensorNamesCount);
  for(int i = 0; i < sensorList->SensorCount; i++){
    temperature[i] = ReadTemperature(sensorList->Sensors[i]);
    switch (i)
    {
    case 0:
      dataTempSensor1 = temperature[i];
      snprintf(acTxt,MAX_STR,"%02f",dataTempSensor1);
      gslc_ElemSetTxtStr(&m_gui,pElemSensorData1,acTxt);
      break;
    case 1:
      dataTempSensor2 = temperature[i];
      snprintf(acTxt,MAX_STR,"%02f",dataTempSensor2);
      gslc_ElemSetTxtStr(&m_gui,pElemSensorData2,acTxt);
      break;
    case 2:
      dataTempSensor3 = temperature[i];
      snprintf(acTxt,MAX_STR,"%02f",dataTempSensor3);
      gslc_ElemSetTxtStr(&m_gui,pElemSensorData3,acTxt);
      break;
    default:
      break;
    }
  }

  dataTempRead = (temperature[0] + temperature[1] + temperature[2]) / 3;
  //Start Stop Process (control the set temperature within the set time)
  if (startStopStatus == 1){
    preheatStatus = 0;
    // Start Counting down
    if (prevMin != minutechecker.tm_min){
      if(dataTimeDurationM <= 0 && dataTimeDurationH > 0){
        dataTimeDurationM = 59;
        dataTimeDurationH--;
      }
      else if (dataTimeDurationM>0){
        dataTimeDurationM--;
      }
      prevMin = minutechecker.tm_min;
    }
    //If time is over, stop the start stop status and shut down all perifirals
    //else control the set temperature until the heating time is expired
    if (dataTimeDurationM <= 0 && dataTimeDurationH <= 0){
      startStopStatus = 0;
      prevMin = 70;
      digitalWrite(pinLed, HIGH);
      digitalWrite(pinFanInternal, HIGH);
      digitalWrite(pinFanOut, HIGH);
      digitalWrite(pinHeater, HIGH);
      digitalWrite(pinPrinter, LOW);
    }
    else{
      regulateHeat();
    }
  }

  if (preheatStatus == 1){
    regulateHeat();
  }
}

int main( int argc, char* args[] )
{
  wiringPiSetup();
  
  pinMode(pinLed, OUTPUT);
  pinMode(pinFanInternal, OUTPUT);
  pinMode(pinFanOut, OUTPUT);
  pinMode(pinHeater, OUTPUT);
  pinMode(pinPrinter, OUTPUT);

  digitalWrite(pinLed, HIGH);
  digitalWrite(pinFanInternal, HIGH);
  digitalWrite(pinFanOut, HIGH);
  digitalWrite(pinHeater, HIGH);
  digitalWrite(pinPrinter, LOW);

  bool              bOk = true;
  gslc_tsElemRef*   pElemRef = NULL;
  char              acTxt[100];

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
    (gslc_tsRect){370,200,100,50},"+ TEMP",0,E_FONT_BTN,&CbBtnMaxTempPlus);
  
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_MaxTempMinus,E_PG_MAIN,
    (gslc_tsRect){180,200,100,50},"- TEMP",0,E_FONT_BTN,&CbBtnMaxTempMinus);    

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForPLA,E_PG_MAIN,
    (gslc_tsRect){215,140,100,50},"PLA",0,E_FONT_BTN,&CbBtnLoadDeafaultForPLA);
 
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_DeafaultForABS,E_PG_MAIN,
    (gslc_tsRect){335,140,100,50},"ABS",0,E_FONT_BTN,&CbBtnLoadDeafaultForABS);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_BTN_StartStop,E_PG_MAIN,
    (gslc_tsRect){180,10,290,55},StartStop,0,E_FONT_BTN,&CbBtnStartStop);

  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_PreHeat,E_PG_MAIN,
    (gslc_tsRect){180,260,290,50},"Preheat",0,E_FONT_BTN,&CbBtnPreHeat);

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

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_DATATEMPMAX,E_PG_MAIN,(gslc_tsRect){288,200,38,50},
    "",0,E_FONT_TXT);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){324,200,25,50},
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

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){150,130,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_SensorData1,E_PG_MAIN,(gslc_tsRect){90,130,25,20},
    "",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,150,25,20},
    "Sensor 2:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){150,150,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_SensorData2,E_PG_MAIN,(gslc_tsRect){90,150,25,20},
    "",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,170,25,20},
    "Sensor 3:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){150,170,25,20},
    "~C",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

    pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_SensorData3,E_PG_MAIN,(gslc_tsRect){90,170,25,20},
    "",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  pElemRef = gslc_ElemCreateTxt(&m_gui,GSLC_ID_AUTO,E_PG_MAIN,(gslc_tsRect){5,205,25,20},
    "Chamber average:",0,E_FONT_TXTSMALL);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GRAY_LT2);

  gslc_tsElemRef*  pElemDataTimeH     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATIMEH);
  gslc_tsElemRef*  pElemDataTimeM     = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATIMEM);
  gslc_tsElemRef*  pElemDataTempMax   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATEMPMAX);
  gslc_tsElemRef*  pElemDataTempRead  = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_DATATEMPREAD);

  gslc_tsElemRef*  pElemSensorData1   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SensorData1);
  gslc_tsElemRef*  pElemSensorData2   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SensorData2);
  gslc_tsElemRef*  pElemSensorData3   = gslc_PageFindElemById(&m_gui,E_PG_MAIN,E_ELEM_SensorData3);

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