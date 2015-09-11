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
int size_menu=4;
String menu[4];

int size_menumode=4;
String menumode[4];

int size_menuset=5;
String menuset[5];

int size_menufan=3;
String menufan[3];

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



void setup()
{
	
/*------Assing Menu String variables------*/
menu[0]="Mode";
menu[1]="Set";	
menu[2]="Exit";
menu[3]="Fan";

menumode[0]="Heat";
menumode[1]="Cool";
menumode[2]="Off";
menumode[3]="Exit";

menuset[0]="Swing";
menuset[1]="Step";
menuset[2]="Min";
menuset[3]="Max";
menuset[4]="Exit";

menufan[0]="On";
menufan[1]="Auto";
menufan[2]="Exit";
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
ScreenTargetTempText.print("TARGET");							//print small text below the target temp
ConfiglAC.setAmbientTemp(TemperatureSensor.getTempCByIndex(0));	//go get ambient temp
ControlAC.setAmbientTemp(ConfiglAC.getAmbientTemp());			//pass the variable to ControlAC so it can later calculate
ScreenAmbientTemp.print(ConfiglAC.getAmbientTemp());			//Print the ambient temp
ScreenAmbientTempText.print("AMBIENT");							//print the small text below ambient temp

}

void loop()
{
	
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
			principalMenu();
			exitingMenu();	
		}

	
	lastKnobPush = knobPush.getStatus();
	
}


/*------------------------------------Functions------------------------------------*/

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
		ConfiglAC.setTargetTemp(ConfiglAC.getTargetTemp() + ConfiglAC.getStepTemp());
		ControlAC.setTargetTemp(ConfiglAC.getTargetTemp());
		ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
	}
	if (rot == -1)
	{	
		ConfiglAC.setTargetTemp(ConfiglAC.getTargetTemp() -  ConfiglAC.getStepTemp());
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
	ScreenTargetTempText.print("TARGET");
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

void principalMenu()
{
	
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
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

void modeMenu()
{
	
	menuPrintLeftMainRight(menumode);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
	
		switch (rotation())
		{
			
		case -1:
		arrayshiftCW(menumode,size_menumode);
		menuPrintLeftMainRight(menumode);
		break;
		
		case 1:
		arrayshiftCWW(menumode,size_menumode);
		menuPrintLeftMainRight(menumode);
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
void setMenu()
{
	
	menuPrintLeftMainRight(menuset);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		switch (rotation())
		{
			case -1:
			arrayshiftCW(menuset,size_menuset);
			menuPrintLeftMainRight(menuset);
			break;
			
			case 1:
			arrayshiftCWW(menuset,size_menuset);
			menuPrintLeftMainRight(menuset);
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
	
void fanMenu()
{
	
	menuPrintLeftMainRight(menufan);
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
		
		switch (rotation())
		{
			
			case -1:
			arrayshiftCW(menufan,size_menufan);
			menuPrintLeftMainRight(menufan);
			break;
			
			case 1:
			arrayshiftCWW(menufan,size_menufan);
			menuPrintLeftMainRight(menufan);
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

void stepMenu(){
	ScreenTargetTemp.print(ConfiglAC.getStepTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
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

void deltaMenu(){
	ScreenTargetTemp.print(ConfiglAC.getDeltaTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
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

void maxTempMenu(){
	ScreenTargetTemp.print(ConfiglAC.getMaxTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
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

void minTempMenu(){
	ScreenTargetTemp.print(ConfiglAC.getMinTemp());
	while (menuExit != true)
	{
		lastKnobPush = knobPush.getStatus();
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
		modeMenu();
		break;
		
		case 2:
		setMenu();
		break;
		
		case 3:
		fanMenu();
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
	menuvar = menumode[1];
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
	menuvar = menufan[1];
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
	menuvar = menuset[1];
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
		deltaMenu();
		menuExit = true;
		break;
		
		case 2:
		maxTempMenu();
		menuExit = true;
		break;
		
		case 3:
		minTempMenu();
		menuExit = true;
		break;
		
		case 4:
		stepMenu();
		menuExit = true;
		break;
		
		default:
		
		break;
	}
}
