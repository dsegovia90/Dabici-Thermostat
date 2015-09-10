#ifndef AcConfig_h
#define AcConfig_h

#include "Arduino.h"

class AcConfig
{
	public:
	void begin();// Assigns default users settings
	
	void setDeltaTemp(float deltaTemp); //Delta in C or F to add or subtract to comparison targetTemp vs ambientTemp
	float getDeltaTemp(); //Returns deltaTemp
	
	void setStepTemp(float stepTemp); 
	float getStepTemp(); 
	
	void setMaxTemp(float maxTemp);
	float getMaxTemp();
	
	void setMinTemp(float minTemp);
	float getMinTemp();
	
	void setAmbientTemp(float ambientTemp); //Set Ambient Temp
	float getAmbientTemp(); //Return Ambient Temp
	
	void setAmbientTempInterval(unsigned long ambientTempInterval);
	unsigned long getAmbientTempInterval();
	
	void setTargetTemp(float targetTemp);
	float getTargetTemp(); //Return TargetTemp
	
	void setACMode(int acMode);
	int getACMode(); //Return AC Mode

	private:
	float _deltaTemp;
	float _stepTemp;
	float _maxTemp;
	float _minTemp;
	float _ambientTemp;
	unsigned long _ambientTempInterval;
	float _targetTemp;
	int _acMode;
};

#endif