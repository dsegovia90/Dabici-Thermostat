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

#define ONE_WIRE_BUS 7

//Encoder pins
Encoder knob(2,3);

int clockOrCounterClock;
bool lastKnobPush = true;


//Thermostat Global Variables


bool menuExit = false;
unsigned long timerToCheckAmbientTemp = 0;
unsigned long timerToSwitchMode = 0;
unsigned long acProtectionTimer = 0;

String menuvar;

int size_menu=4;
String menu[4];

int size_menumode=4;
String menumode[4];

int size_menuset=5;
String menuset[5];

int size_menufan=3;
String menufan[3];
//////////////////////////////////////////////////////////////////////////////////////

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature TemperatureSensor(&oneWire);

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


PushButton knobPush(4);

///Create default settings///
AcConfig  ConfiglAC;
//Assign Control Outputs///
AcControl ControlAC(15,14,16); //Pin A1, A0, A2 respectively
/////////////////////////////////////

int lastModeSetting=0;



void setup()
{
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

		
///Create default settings///
ConfiglAC.begin();
int lastModeSetting = ConfiglAC.getACMode();
///Load default settings to control///
ControlAC.begin(ConfiglAC.getACMode(),ConfiglAC.getAmbientTemp(),ConfiglAC.getTargetTemp(),ConfiglAC.getDeltaTemp(),ConfiglAC.getAmbientTempInterval());
TemperatureSensor.begin();
tft.begin();
knobPush.begin();
delay(200);
TemperatureSensor.setResolution(11);
TemperatureSensor.requestTemperatures();
tft.fillScreen(ILI9340_BLACK);
tft.setRotation(3);
//ScreenMode.print((String)ConfiglAC.getACMode());
printACMode();
ScreenTargetTempText.print("TARGET");
ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
ConfiglAC.setAmbientTemp(TemperatureSensor.getTempCByIndex(0));
ControlAC.setAmbientTemp(ConfiglAC.getAmbientTemp());
ScreenAmbientTemp.print(ConfiglAC.getAmbientTemp());
ScreenAmbientTempText.print("AMBIENT");
 

}

void loop()
{
changeWishedTemp(rotation());

	if (timerToCheckAmbientTemp + ConfiglAC.getAmbientTempInterval() <= millis())
	{
		TemperatureSensor.requestTemperatures();
		ConfiglAC.setAmbientTemp(TemperatureSensor.getTempCByIndex(0));
		ControlAC.setAmbientTemp(ConfiglAC.getAmbientTemp());
		ScreenAmbientTemp.print(ConfiglAC.getAmbientTemp());
		timerToCheckAmbientTemp = millis();
	}

ControlAC.activateACMode();

	if (knobPush.getStatus() == false && lastKnobPush == true) // Enter Menu
	{
		delay(20);
		enteringMenu();
		menuExit = false;
		int shiftVar=0;
		principalMenu();
		exitingMenu();
	}



// Exiting Menu
lastKnobPush = knobPush.getStatus();






}



//Functions /////////////////////////////////////////////////////////////////

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


int rotation() //Monitors Changes in the Knob via external Interrupts (Returns "CW" for clockwise and "CCW" for counter clockwise; otherwise returns "IDLE")
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

void changeWishedTemp(int rot) //Adds or subtracts 0.5 to targetTemp
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

void changeModeSetting(int rot) //ModeSetting means "OFF", "HEAT' or "COOL"
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

void enteringMenu() //What to do as the program enters into "Menu Mode"
{
	ScreenTargetTemp.eraseFloat();
	ScreenTargetTempText.eraseString();
	ScreenMenuMain.print(menu[1]);
	ScreenMenuRight.print(menu[2]);
	ScreenMenuLeft.print(menu[0]);
	delay(200);
}

void exitingMenu() // What to do as the program exits "Menu Mode"
{
	ScreenMenuMain.eraseString();
	ScreenMenuRight.eraseString();
	ScreenMenuLeft.eraseString();
	
	ScreenTargetTemp.print(ConfiglAC.getTargetTemp());
	ScreenTargetTempText.print("TARGET");
	printACMode();
	delay(200);
}

void menuPrintLeftMainRight(String* array)
{
	ScreenMenuMain.print(array[1]);
	ScreenMenuRight.print(array[2]);
	ScreenMenuLeft.print(array[0]);
}

void arrayshiftCW(String *array, int size)
{
	int shiftVar = 0;
	menuvar = array[0];
	for (shiftVar = 0; shiftVar<size-1; shiftVar++){
	
	array[shiftVar] = array[shiftVar+1];}
	array[size-1] = menuvar;
}

void arrayshiftCWW(String *array, int size)
{
	int shiftVar = 0;
	menuvar = array[size-1];
	for (shiftVar = size-1; shiftVar >= 0; shiftVar--){
	array[shiftVar] = array[shiftVar-1];}
	array[0] = menuvar;
	
}

//////////////////////////////////////Menus////////////////////////////////

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

//////////////////////////////Menu selects////////////////////////////////////////
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
