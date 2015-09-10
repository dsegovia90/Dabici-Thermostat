#ifndef SegoACControl_h
#define SegoACControl_h

#include "Arduino.h"

class SegoACControl
{
	public:
	SegoACControl(int fanPin, int coolPinStg1, int heatPinStg1);
	
	void begin(); //Set pins to output and set the default setProtectACTimer to 60000 millis
	void setProtectACTimerDelta(unsigned int protectACTimerDelta); //After turning off, this is the protection timer to wait for turn on again default is 60000 milliseconds or 1 minute
	unsigned long getProtectACTimerDelta(); //Returns the protectACTimer set before (default 1 minute)
	void setProtectACTimer(unsigned int protectACTimer); //After turning off, this is the protection timer to wait for turn on again default is 60000 milliseconds or 1 minute
	unsigned long getProtectACTimer(); //Returns the protectACTimer set before (default 1 minute)
	
	void setDeltaTemp(float deltaTemp); //Delta in C or F to add or subtract to comparison targetTemp vs ambientTemp
	float getDeltaTemp(); //Returns deltaTemp
	void setAmbientTemp(float ambientTemp); //Set Ambient Temp
	float getAmbientTemp(); //Return Ambient Temp
	
	void setAmbientTempInterval(unsigned long ambientTempInterval);
	unsigned long getAmbientTempInterval();
	
	void setTargetTemp(float targetTemp);
	float getTargetTemp(); //Return TargetTemp
	
	void setACMode(int acMode);
	int getACMode(); //Return AC Mode
	
	void activateACMode();
	String stringACMode();
	
	private:
	int _fanPin;
	int _coolPinStg1;
	int _heatPinStg1;
	unsigned int _protectACTimer;
	unsigned int _protectACTimerDelta;
	float _deltaTemp;
	float _ambientTemp;
	unsigned long _ambientTempInterval;
	float _targetTemp;
	
	int _acMode;
	int _acModeMax;
	int _acMenu;
	int _acMenuMax;
	
	bool _acProtection;
	
	String _stringACMode;
	
};

#endif