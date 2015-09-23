/*
  *@Dabici Labs © 
  * @file    main.c
  * @author-1  Daniel Segovia
  * @author-2  Diego Cepeda
  * @version V 0.2
  * @date    11-September-2015
  * @brief   Intelligent Thermostat
*/

/*------Includes------*/
#include "Encoder.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "ThermostatScreen.h"
#include "SegoPushButton.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "AcConfig.h"
#include "AcControl.h"
#include "SoftwareSerial.h"

/*------Defines------*/
#define ONE_WIRE_BUS 7

/*------Encoder Pins Assign------*/
Encoder knob(2,3);

/*------Push Button Variables------*/
int clockOrCounterClock;
bool lastKnobPush = true;
/*------Push Button Pin Assign------*/
PushButton knobPush(4);



/*------Thermostat Global Variables------*/

unsigned long timerToCheckAmbientTemp = 0;			//initialize this timer in 0 to later compare to actual time + delta
unsigned long timerToSwitchMode = 0;				//initialize this timer in 0
unsigned long acProtectionTimer = 0;				//initialize this timer in 0

/*------Multi Purpose Menu Variables------*/
bool menuExit = false;
String menuvar;


/*------Initialize Menu Arrays and Sizes------*/
int const size_menu=4;
String menu[size_menu];

int const size_menuMode=4;
String menuMode[size_menuMode];

int const size_menuSet=5;
String menuSet[size_menuSet];

int const size_menuFan=3;
String menuFan[size_menuFan];

int const size_menuReference=2;
String menuReference[size_menuReference];

/*------Initialize Temperature Sensor------*/
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature TemperatureSensor(&oneWire);

/*------Initialize Display Settings------*/
ThermostatScreen ScreenMode(200,200,4,ILI9340_WHITE);
ThermostatScreen ScreenTargetTemp(50,100,7,ILI9340_WHITE);
ThermostatScreen ScreenTargetTempText(217,155,1,ILI9340_WHITE);
ThermostatScreen ScreenMenuMain(100,100,4,ILI9340_WHITE);
ThermostatScreen ScreenMenuLeft(10,115,2,ILI9340_WHITE);
ThermostatScreen ScreenMenuRight(240,115,2,ILI9340_WHITE);
ThermostatScreen ScreenModeSetting(50,100,7,ILI9340_WHITE);
ThermostatScreen ScreenAmbientTemp(20,20,4,ILI9340_WHITE);
ThermostatScreen ScreenAmbientTempText(95,53,1,ILI9340_WHITE);
ThermostatScreen ScreenAcProtection(200,40,1,ILI9340_WHITE);



/*------Create AC Configuration Object------*/

AcConfig  ConfiglAC;

/*------Assign AC Control Outputs------*/
AcControl ControlAC(15,14,16); //Pin A1, A0, A2 respectively


int lastModeSetting=0;

unsigned long time_ReadN;
unsigned long time_ReadO;
unsigned long time_ReadS;
/*------Assing WIFI String variables------*/
String data;
String setting;
String set;

void setup()
{


/*------Begin Serial------*/	
 Serial.begin(9600);
 Serial.println("Start");
/*------Assing Menu String variables------*/

menu[0]="Mode";
menu[1]="Set";	
menu[2]="Exit";
menu[3]="Fan";

menuMode[0]="Heat";
menuMode[1]="Cool";
menuMode[2]="Off";
menuMode[3]="Exit";

menuSet[0]="Swing";
menuSet[1]="Step";
menuSet[2]="Min";
menuSet[3]="Max";
menuSet[4]="Exit";

menuFan[0]="On";
menuFan[1]="Auto";
menuFan[2]="Exit";

menuReference[0] = "AMBIENT";
menuReference[1] = "TARGET";
/*------------------------------------------*/

/*------Create default AC Configuration------*/		
ConfiglAC.begin();
int lastModeSetting = ConfiglAC.getACMode();

/*------Load Default Settings to Control------*/		
ControlAC.begin(ConfiglAC.getACMode(),ConfiglAC.getAmbientTemp(),ConfiglAC.getTargetTemp(),ConfiglAC.getDeltaTemp(),ConfiglAC.getAmbientTempInterval());


TemperatureSensor.begin();					//Initialize communication with temp sensor
tft.begin();								//Initialize display
knobPush.begin();							//Initialize knob's push button
delay(200);									//Delay
TemperatureSensor.setResolution(11);		//Set resolution of temp sensor (takes about 500ms to check @ resulution =11)
TemperatureSensor.requestTemperatures();	//Request initial temperature to use in setup
tft.fillScreen(ILI9340_BLACK);				//Fill screen to black (background color)
tft.setRotation(3);							//Rotate Screen 3x90°
printACMode();													//print ConfiglAC.getACMode() on ScreenMode.print()
ScreenTargetTemp.print(ConfiglAC.getTargetTemp());				//print the target temp
ScreenTargetTempText.print(menuReference[1]);							//print small text below the target temp
ConfiglAC.setAmbientTemp(TemperatureSensor.getTempCByIndex(0));	//go get ambient temp
ControlAC.setAmbientTemp(ConfiglAC.getAmbientTemp());			//pass the variable to ControlAC so it can later calculate
ScreenAmbientTemp.print(ConfiglAC.getAmbientTemp());			//Print the ambient temp
ScreenAmbientTempText.print(menuReference[0]);							//print the small text below ambient temp

}

void loop()
{	
	if (Serial.available()){
		data=(String)Serial.readString();
		Serial.println(data);
		setting=data.substring(0,4);
		set=data.substring(5,7);
		
		
		
			if(setting=="maxt"){
				int coolval= set.toInt();
				if(coolval>=30){coolval=30;}
				if(coolval<=18){coolval=18;}
				ConfiglAC.setMaxTemp(coolval);
					
			}
			if(setting=="mint"){
				int coolval= set.toInt();
				if(coolval>=30){coolval=30;}
				if(coolval<=18){coolval=18;}
				ConfiglAC.setMinTemp(coolval);
				
			}
		
		
		if(setting=="delt"){
			int coolval= set.toInt();
			switch(coolval){
			
			case 1 :
			ConfiglAC.setDeltaTemp(.25);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
			break;
			case 2 :
			ConfiglAC.setDeltaTemp(.5);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
			break;
			case 3 :
			ConfiglAC.setDeltaTemp(.75);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
			break;
			case 4 :
			ConfiglAC.setDeltaTemp(1);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
			break;
			
			 default:
		
			 break;
			}
			
		}
		
		if(setting=="step"){
			int coolval= set.toInt();
			switch(coolval){
				
				case 1 :
				ConfiglAC.setStepTemp(.25);
				break;
				case 2 :
				ConfiglAC.setStepTemp(.5);				
				break;
				case 3 :
				ConfiglAC.setStepTemp(.75);				
				break;
				case 4 :
				ConfiglAC.setStepTemp(1);				
				break;
				
				default:
				
				break;
			}
			
		}
		
		
		if(setting=="ooff"){
			ConfiglAC.setACMode(0);
			ControlAC.setACMode(ConfiglAC.getACMode());
		}
		
		if(setting=="cool"){
			int coolval= set.toInt();
			if(coolval>=ConfiglAC.getMaxTemp()){coolval=ConfiglAC.getMaxTemp();}
			if(coolval<=ConfiglAC.getMinTemp()){coolval=ConfiglAC.getMinTemp();}
			ConfiglAC.setTargetTemp(coolval);
			ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
			ConfiglAC.setACMode(1);
			ControlAC.setACMode(ConfiglAC.getACMode());
			
			
		}
		
		if(setting=="heat"){
			int heatval= set.toInt();
			int coolval= set.toInt();
			if(coolval>=ConfiglAC.getMaxTemp()){coolval=ConfiglAC.getMaxTemp();}
			if(coolval<=ConfiglAC.getMinTemp()){coolval=ConfiglAC.getMinTemp();}	
			ConfiglAC.setTargetTemp(coolval);
			ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
			ConfiglAC.setACMode(2);
			ControlAC.setACMode(ConfiglAC.getACMode());
			
			
			
		}
		
		if(setting=="fann"){
			
			int fanval= set.toInt();
			int coolval= set.toInt();
			if(coolval==1){ConfiglAC.setACMode(3);}
				else{ConfiglAC.setACMode(0);}
			
			
			ControlAC.setACMode(ConfiglAC.getACMode());
			
		}
		printACMode();
		ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
		}
		
	
		
	checkTimeProtection();	//Check the status since las protection mode in AC
	
	changeWishedTemp(rotation());	//Check if there is movement in the knob, if there is add or subtract depending on the deltaStep

	if (timerToCheckAmbientTemp + ConfiglAC.getAmbientTempInterval() <= millis())	//how often to go check for temp in sensor (depends of Config AC variable in milliseconds)
	{
		TemperatureSensor.requestTemperatures();									//request temp
		ConfiglAC.setAmbientTemp(TemperatureSensor.getTempCByIndex(0));				//update temp to ConfigAC
		ControlAC.setAmbientTemp(ConfiglAC.getAmbientTemp());						//update temp to ControlAC for it to compare
		ScreenAmbientTemp.print(ConfiglAC.getAmbientTemp());						//print the temp to screen
		timerToCheckAmbientTemp = millis();											//set the timer to actual time
	}


		ControlAC.activateACMode();		//Checks and compares all the data necessary to turn on or off the outputs to the AC

		if (knobPush.getStatus() == false && lastKnobPush == true) // Enter Menu
		{
			delay(20);				//Delay 20 ms
			enteringMenu();
			menuExit = false;
			int shiftVar=0;
			
			principalMenu(millis());
			exitingMenu();	
		}
;
	
	lastKnobPush = knobPush.getStatus();
	
}


/*------------------------------------Functions------------------------------------*/


/***********************************************************************************/
// Function Name  : checkTimeProtection
// Description    : Checks the status since las protection mode in AC
// Parameters	  : None
// Returns        : None
/***********************************************************************************/

void checkTimeProtection(){
	
	if(ControlAC.getAcProtection()==false){
		time_ReadO=millis();
		
	}

	if(ControlAC.getAcProtection()==true){
		time_ReadN=millis();
		time_ReadS=time_ReadN-time_ReadO;
	}

	if(time_ReadS>=10000){
		ControlAC.setAcProtection(false);
		
		time_ReadO=0;
		time_ReadS=0;
	}
}

/***********************************************************************************/
// Function Name  : printACMode													   
// Description    : Print actual Ac Mode on Screen
// Parameters	  : None
// Returns        : None
/***********************************************************************************/

void printACMode(){
	
	switch (ConfiglAC.getACMode())
	{
		
		case 0:
		ScreenMode.print("OFF");
		break;
		
		case 1:
		ScreenMode.print("COOL");
		break;
		
		case 2:
		ScreenMode.print("HEAT");
		break;
		
		case 3:
		ScreenMode.print("FAN");
		break;
		
		default:
		
		break;
	}
	
}
/**************************************************************************************************************************************************************/
// Function Name  : rotation
// Description    : Monitors Changes in the Knob via external Interrupts (Returns   "1" for clockwise and "-1" for counter clockwise; otherwise Returns   "0")
// Parameters     : None
// Returns  	  : int
/*************************************************************************************************************************************************************/

int rotation()
{
	
	if ((knob.read()/4) > 0)
	{
		clockOrCounterClock = -1;//counter clockwise
		knob.write(0);
		
	}
	
	else if ((knob.read()/4) < 0)
	{
		clockOrCounterClock = 1;//clockwise
		knob.write(0);
	}
		
		else
		{
		clockOrCounterClock = 0;//idle
		}
	
	return clockOrCounterClock;
	
}

/*********************************************************************************************************************/
// Function Name  : changeWishedTemp
// Description    : Adds or subtracts 0.5 to targetTemp depending on the rotation value given from rotation() function 
// Parameters	  : int rotation
// Returns        : None
/*********************************************************************************************************************/

void changeWishedTemp(int rot) 
{
	if (rot == 1)
	{
		
		if(ConfiglAC.getTargetTemp() + ConfiglAC.getStepTemp()>=ConfiglAC.getMaxTemp()){
			ConfiglAC.setTargetTemp(ConfiglAC.getMaxTemp());
		}
		else{
		ConfiglAC.setTargetTemp(ConfiglAC.getTargetTemp() + ConfiglAC.getStepTemp());}
		
		ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
		ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
	}
	if (rot == -1)
	{	
		if(ConfiglAC.getTargetTemp() - ConfiglAC.getStepTemp()<=ConfiglAC.getMinTemp()){
			ConfiglAC.setTargetTemp(ConfiglAC.getMinTemp());
		}
		else{
		ConfiglAC.setTargetTemp(ConfiglAC.getTargetTemp() -  ConfiglAC.getStepTemp());}
		
		ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
		ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
	}
	if (rot == 0)
	{
		
		ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
		
	}
}

/*********************************************************************************************************************/
// Function Name  : changeWishedStep
// Description    : Adds or subtracts 0.25 to stepTemp depending on the rotation value given from rotation() function
// Parameters	  : int rotation
// Returns        : None
/*********************************************************************************************************************/

void changeWishedStep(int rot) 
{
	if (rot == 1)
	{	
		if (ConfiglAC.getStepTemp()>.75)
		{
			ConfiglAC.setStepTemp(1);
		}
		
		else{
			ConfiglAC.setStepTemp(ConfiglAC.getStepTemp() + .25);
		}
		ScreenTargetTemp.print(ConfiglAC.getStepTemp());
	}
	if (rot == -1)
	{	if (ConfiglAC.getStepTemp()<.5)
		{
			ConfiglAC.setStepTemp(.25);
		}
		
		else{
			ConfiglAC.setStepTemp(ConfiglAC.getStepTemp() - .25);
		}
		ScreenTargetTemp.print(ConfiglAC.getStepTemp());
	}
	if (rot == 0)
	{
		ConfiglAC.setStepTemp(ConfiglAC.getStepTemp());	
	}	
}

/*********************************************************************************************************************/
// Function Name  : changeWishedDelta
// Description    : Adds or subtracts 0.25 to deltaTemp depending on the rotation value given from rotation() function
// Parameters	  : int rotation
// Returns        : None
/*********************************************************************************************************************/

void changeWishedDelta(int rot) 
{
	if (rot == 1)
	{
		if (ConfiglAC.getDeltaTemp()>.75)
		{
			ConfiglAC.setDeltaTemp(1);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
		}
		
		else{
			ConfiglAC.setDeltaTemp(ConfiglAC.getDeltaTemp() + .25);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
		}
		ScreenTargetTemp.print(ConfiglAC.getDeltaTemp());
	}
	if (rot == -1)
	{	if (ConfiglAC.getDeltaTemp()<.5)
		{
			ConfiglAC.setDeltaTemp(.25);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
		}
		
		else{
			ConfiglAC.setDeltaTemp(ConfiglAC.getDeltaTemp() - .25);
			ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
		}
		ScreenTargetTemp.print(ConfiglAC.getDeltaTemp());
	}
	if (rot == 0)
	{
		ConfiglAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
		ControlAC.setDeltaTemp(ConfiglAC.getDeltaTemp());
	}
}

/*********************************************************************************************************************/
// Function Name  : changeWishedMaxTemp
// Description    : Adds or subtracts stepTemp to maxTemp depending on the rotation value given from rotation() function
// Parameters	  : int rotation
// Returns        : None
/*********************************************************************************************************************/

void changeWishedMaxTemp(int rot)
{
	if (rot == 1)
	{
		if (ConfiglAC.getMaxTemp()>29)
		{
			ConfiglAC.setMaxTemp(30);
			
		}
		
		else{
			ConfiglAC.setMaxTemp(ConfiglAC.getMaxTemp() + ConfiglAC.getStepTemp());
			
		}
		ScreenTargetTemp.print(ConfiglAC.getMaxTemp());
	}
	if (rot == -1)
	{	if (ConfiglAC.getMaxTemp()<25.1)
		{
			ConfiglAC.setMaxTemp(25);
			
		}
		
		else{
			ConfiglAC.setMaxTemp(ConfiglAC.getMaxTemp() - ConfiglAC.getStepTemp());
			
		}
		ScreenTargetTemp.print(ConfiglAC.getMaxTemp());
	}
	if (rot == 0)
	{
		ConfiglAC.setMaxTemp(ConfiglAC.getMaxTemp());
		
	}
}

/*********************************************************************************************************************/
// Function Name  : changeWishedMinTemp
// Description    : Adds or subtracts stepTemp to minTemp depending on the rotation value given from rotation() function
// Parameters	  : int rotation
// Returns        : None
/*********************************************************************************************************************/

void changeWishedMinTemp(int rot)
{
	if (rot == 1)
	{
		if (ConfiglAC.getMinTemp()>23)
		{
			ConfiglAC.setMinTemp(24);
			
		}
		
		else{
			ConfiglAC.setMinTemp(ConfiglAC.getMinTemp() + ConfiglAC.getStepTemp());
			
		}
		ScreenTargetTemp.print(ConfiglAC.getMinTemp());
	}
	if (rot == -1)
	{	if (ConfiglAC.getMinTemp()<18.1)
		{
			ConfiglAC.setMinTemp(18);
			
		}
		
		else{
			ConfiglAC.setMinTemp(ConfiglAC.getMinTemp() - ConfiglAC.getStepTemp());
			
		}
		ScreenTargetTemp.print(ConfiglAC.getMinTemp());
	}
	if (rot == 0)
	{
		ConfiglAC.setMinTemp(ConfiglAC.getMinTemp());
		
	}
}

/*****************************************************************************************************************************************************/
// Function Name  : changeModeSetting
// Description    : Depending on the rotation value given from rotation() function it alternates the menu between ModeSetting  "OFF", "HEAT' or "COOL"
// Parameters	  : int rotation
// Returns        : None
/*****************************************************************************************************************************************************/

void changeModeSetting(int rot) 
{
	if (rot == 1)
	{
		ControlAC.setACMode(ConfiglAC.getACMode() + 1);
	}
	else if(rot == -1)
	{
		ControlAC.setACMode(ConfiglAC.getACMode() - 1);
	}
}

/*********************************************************************/
// Function Name  : enteringMenu
// Description    : What to do as the program enters into "Menu Mode"
// Parameters	  : None
// Returns        : None
/********************************************************************/

void enteringMenu() 
{
	ScreenTargetTemp.eraseFloat();
	ScreenTargetTempText.eraseString();
	ScreenMenuMain.print(menu[1]);
	ScreenMenuRight.print(menu[2]);
	ScreenMenuLeft.print(menu[0]);
	delay(200);
}

/******************************************************************/
// Function Name  : exitingMenu
// Description    : What to do as the program exits "Menu Mode"
// Parameters	  : None
// Returns        : None
/*****************************************************************/

void exitingMenu() 
{
	ScreenMenuMain.eraseString();
	ScreenMenuRight.eraseString();
	ScreenMenuLeft.eraseString();
	
	ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
	ScreenTargetTempText.print(menuReference[1]);
	printACMode();
	delay(200);
}

/*******************************************/
// Function Name  : menuPrintLeftMainRight
// Description    : prints menu
// Parameters	  : None
// Returns        : None
/*******************************************/

void menuPrintLeftMainRight(String* array)
{
	ScreenMenuMain.print(array[1]);
	ScreenMenuRight.print(array[2]);
	ScreenMenuLeft.print(array[0]);
}

/***************************************************/
// Function Name  : arrayshiftCW
// Description    : Shifts the giver array clockwise
// Parameters	  : *String,int
// Returns        : None
/***************************************************/

void arrayshiftCW(String *array, int size)
{
	int shiftVar = 0;
	menuvar = array[0];
	for (shiftVar = 0; shiftVar<size-1; shiftVar++){
	
	array[shiftVar] = array[shiftVar+1];}
	array[size-1] = menuvar;
}

/***********************************************************/
// Function Name  : arrayshiftCWW
// Description    : Shifts the giver array counter clockwise
// Parameters	  : *String,int
// Returns        : None
/***********************************************************/

void arrayshiftCWW(String *array, int size)
{
	int shiftVar = 0;
	menuvar = array[size-1];
	for (shiftVar = size-1; shiftVar >= 0; shiftVar--){
	array[shiftVar] = array[shiftVar-1];}
	array[0] = menuvar;
	
}

/*------------------------------------Menus------------------------------------*/

/***********************************************************/
// Function Name  : principalMenu
// Description    : Principal Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void principalMenu(unsigned long times)
{

	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		if(millis()-times >= 10000){
			exitingMenu();
			menuExit=true;
		}
		switch (rotation())
		{
			
			case -1:
			arrayshiftCW(menu,size_menu);
			menuPrintLeftMainRight(menu);
			break;
			
			case 1:
			arrayshiftCWW(menu,size_menu);
			menuPrintLeftMainRight(menu);
			break;
			
			default:
			
			break;
		}
		
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
		
		delay(20);
		ScreenMenuMain.eraseString();
		ScreenMenuRight.eraseString();
		ScreenMenuLeft.eraseString();
		menuSelect1();
		
		
		}
	}
}

/***********************************************************/
// Function Name  : modeMenu
// Description    : Mode Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void modeMenu(unsigned long times)
{
	
	menuPrintLeftMainRight(menuMode);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		if(millis()-times >= 10000){
		exitingMenu();
		menuExit=true;
		}
		
		switch (rotation())
		{
			
		case -1:
		arrayshiftCW(menuMode,size_menuMode);
		menuPrintLeftMainRight(menuMode);
		break;
		
		case 1:
		arrayshiftCWW(menuMode,size_menuMode);
		menuPrintLeftMainRight(menuMode);
		break;
		
		default:
		
		break;
		}
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			ScreenMenuMain.eraseString();
			ScreenMenuRight.eraseString();
			ScreenMenuLeft.eraseString();
			modeAssign();
			
		}
		
	}
}

/***********************************************************/
// Function Name  : setMenu
// Description    : Set Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/
void setMenu(unsigned long times)
{
	
	menuPrintLeftMainRight(menuSet);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
			
			if(millis()-times >= 10000){
				exitingMenu();
				menuExit=true;
			}
			
		switch (rotation())
		{
			case -1:
			arrayshiftCW(menuSet,size_menuSet);
			menuPrintLeftMainRight(menuSet);
			break;
			
			case 1:
			arrayshiftCWW(menuSet,size_menuSet);
			menuPrintLeftMainRight(menuSet);
			break;
			
			default:
			
			break;
		}
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			ScreenMenuMain.eraseString();
			ScreenMenuRight.eraseString();
			ScreenMenuLeft.eraseString();
			setAssign();
			
		}
	}
}

/***********************************************************/
// Function Name  : fanMenu
// Description    : Fan Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/
	
void fanMenu(unsigned long times)
{
	
	menuPrintLeftMainRight(menuFan);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
			
			if(millis()-times >= 10000){
				exitingMenu();
				menuExit=true;
			}
			
		switch (rotation())
		{
			
			case -1:
			arrayshiftCW(menuFan,size_menuFan);
			menuPrintLeftMainRight(menuFan);
			break;
			
			case 1:
			arrayshiftCWW(menuFan,size_menuFan);
			menuPrintLeftMainRight(menuFan);
			break;
			
			default:
			
			break;
		}
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			ScreenMenuMain.eraseString();
			ScreenMenuRight.eraseString();
			ScreenMenuLeft.eraseString();
			fanAssign();
			
		}
	}
}

/***********************************************************/
// Function Name  : stepMenu
// Description    : Step Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void stepMenu(unsigned long times){
	ScreenTargetTemp.print(ConfiglAC.getStepTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
	
		if(millis()-times >= 10000){
			exitingMenu();
			menuExit=true;
		}
		
		changeWishedStep(rotation());
		delay(20);
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			menuExit=true;
			
		}
	}	
}

/***********************************************************/
// Function Name  : deltaMenu
// Description    : Delta Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void deltaMenu(unsigned long times){
	ScreenTargetTemp.print(ConfiglAC.getDeltaTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		if(millis()-times >= 10000){
			exitingMenu();
			menuExit=true;
		}
		
		changeWishedDelta(rotation());
		delay(20);
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			menuExit=true;
			
		}
	}
}

/***********************************************************/
// Function Name  : maxTempMenu
// Description    : Max Temperature Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void maxTempMenu(unsigned long times){
	ScreenTargetTemp.print(ConfiglAC.getMaxTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		if(millis()-times >= 10000){
			exitingMenu();
			menuExit=true;
		}
		
		changeWishedMaxTemp(rotation());
		delay(20);
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			menuExit=true;
			
		}
	}
}

/***********************************************************/
// Function Name  : minTempMenu
// Description    : Min Temperature Menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void minTempMenu(unsigned long times){
	ScreenTargetTemp.print(ConfiglAC.getMinTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		if(millis()-times >= 10000){
			exitingMenu();
			menuExit=true;
		}
		
		changeWishedMinTemp(rotation());
		delay(20);
		if (knobPush.getStatus() == false && lastKnobPush == true){	 // Enter Menu
			
			delay(20);
			menuExit=true;
			
		}
	}
}

/*------------------------------------Menu Select Routines------------------------------------*/

/***********************************************************/
// Function Name  : menuSelect1
// Description    : Go o the selected menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void menuSelect1()
{
	int select = 0;
	menuvar = menu[1];
	if(menuvar == "Exit") { select = 0; }
	else if(menuvar == "Mode"){ select =1; }
	else if( menuvar == "Set"){ select =2; }
	else if( menuvar == "Fan"){ select =3; }

	switch (select)
	{
		
		case 0:
		menuExit=true;
		break;
		
		case 1:
		modeMenu(millis());
		break;
		
		case 2:
		setMenu(millis());
		break;
		
		case 3:
		fanMenu(millis());
		break;
		
		default:
		
		break;
	}
}

/***********************************************************/
// Function Name  : modeAssign
// Description    : Go o the selected menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void modeAssign()
{
	int select = 0;
	menuvar = menuMode[1];
	if(menuvar == "Exit") { select = 0; }
	else if(menuvar == "Cool"){ select =1; }
	else if( menuvar == "Heat"){ select =2; }
	else if( menuvar == "Off"){ select =3; }

	switch (select)
	{
		
		case 0:
		menuExit = true;
		break;
		
		case 1:
		ConfiglAC.setACMode(1);
		ControlAC.setACMode(ConfiglAC.getACMode());
		printACMode();
		menuExit = true;
		break;
		
		case 2:
		ConfiglAC.setACMode(2);
		ControlAC.setACMode(ConfiglAC.getACMode());
		printACMode();
		menuExit = true;
		break;
		
		case 3:
		ConfiglAC.setACMode(0);
		ControlAC.setACMode(ConfiglAC.getACMode());
		printACMode();
		menuExit = true;
		break;
		
		default:
		
		break;
	}
}

/***********************************************************/
// Function Name  : fanAssign
// Description    : Go o the selected menu
// Parameters	  : None
// Returns        : None
/***********************************************************/
	
void fanAssign()
{
	int select = 0;
	menuvar = menuFan[1];
	if( menuvar == "Exit"){ select =0; }
	else if(menuvar == "Auto"){ select =1; }	
	else if(menuvar == "On") { select = 2; } 


	switch (select)
	{
		
		case 0:
		menuExit = true;
		break;

		case 1:
		ConfiglAC.setACMode(0);
		ControlAC.setACMode(ConfiglAC.getACMode());
		printACMode();
		menuExit = true;
		break;
		
		case 2:
		ConfiglAC.setACMode(3);
		ControlAC.setACMode(ConfiglAC.getACMode());
		printACMode();
		menuExit = true;
		break;
		
		
		default:
		
		break;
	}
	
}

/***********************************************************/
// Function Name  : setAssign
// Description    : Go o the selected menu
// Parameters	  : None
// Returns        : None
/***********************************************************/

void setAssign()
{
	int select = 0;
	menuvar = menuSet[1];
	if( menuvar == "Exit"){ select =0; }
	else if(menuvar == "Swing"){ select =1; }
	else if(menuvar == "Max")  { select = 2; }
	else if(menuvar == "Min")  { select = 3; }
	else if(menuvar == "Step") { select = 4; }


	switch (select)
	{
		case 0:
		menuExit = true;
		break;

		case 1:
		deltaMenu(millis());
		menuExit = true;
		break;
		
		case 2:
		maxTempMenu(millis());
		menuExit = true;
		break;
		
		case 3:
		minTempMenu(millis());
		menuExit = true;
		break;
		
		case 4:
		stepMenu(millis());
		menuExit = true;
		break;
		
		default:
		
		break;
	}
}
