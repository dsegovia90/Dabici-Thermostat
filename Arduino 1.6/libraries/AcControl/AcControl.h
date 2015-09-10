#ifndef AcControl_h
#define AcControl_h

#include "Arduino.h"

class AcControl
{
public:
AcControl(int fanPin, int coolPinStg1, int heatPinStg1);

void begin(int acMode,float ambientTemp,float targetTemp,float deltaTemp,unsigned long ambientTempInterval);// Assigns default users settings

void setDeltaTemp(float deltaTemp); //Delta in C or F to add or subtract to comparison targetTemp vs ambientTemp

void setAmbientTemp(float ambientTemp); //Set Ambient Temp

void setAmbientTempInterval(unsigned long ambientTempInterval);//Set Ambient Temp interval

void setTargetTemp(float targetTemp);//Set Target Temp

void setACMode(int acMode);//Set Ac Mode

void setAcProtection(bool acProtection);//defines if Ac is in  protected mode

boolean getAcProtection();//returns value status to check if protection is needed

void activateACMode();//activate the Ac according to parameters given




private:
int _fanPin;
int _coolPinStg1;
int _heatPinStg1; 

float _deltaTemp;
float _ambientTemp;
unsigned long _ambientTempInterval;
float _targetTemp;
int _acMode;

boolean _acProtection;

};

#endif