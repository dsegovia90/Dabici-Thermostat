/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Uno, Platform=avr, Package=arduino
*/

#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 165
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define F_CPU 16000000L
#define ARDUINO 165
#define ARDUINO_AVR_UNO
#define ARDUINO_ARCH_AVR
extern "C" void __cxa_pure_virtual() {;}

//
//
void checkTimeProtection();
void printACMode();
int rotation();
void changeWishedTemp(int rot);
void changeWishedStep(int rot);
void changeWishedDelta(int rot);
void changeWishedMaxTemp(int rot);
void changeWishedMinTemp(int rot);
void changeModeSetting(int rot);
void enteringMenu();
void exitingMenu();
void menuPrintLeftMainRight(String* array);
void arrayshiftCW(String *array, int size);
void arrayshiftCWW(String *array, int size);
void principalMenu(unsigned long times);
void modeMenu(unsigned long times);
void setMenu(unsigned long times);
void fanMenu(unsigned long times);
void stepMenu(unsigned long times);
void deltaMenu(unsigned long times);
void maxTempMenu(unsigned long times);
void minTempMenu(unsigned long times);
void menuSelect1();
void modeAssign();
void fanAssign();
void setAssign();

#include "C:\Users\Diego\Documents\GitHub\Dabici-Thermostat\Arduino 1.6\hardware\arduino\avr\variants\standard\pins_arduino.h" 
#include "C:\Users\Diego\Documents\GitHub\Dabici-Thermostat\Arduino 1.6\hardware\arduino\avr\cores\arduino\arduino.h"
#include <screenControlv0.2.ino>
