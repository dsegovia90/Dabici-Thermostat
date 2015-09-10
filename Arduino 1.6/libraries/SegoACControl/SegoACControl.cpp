#include "SegoACControl.h"

SegoACControl::SegoACControl(int fanPin, int coolPinStg1, int heatPinStg1)
{
	_fanPin = fanPin;
	_coolPinStg1 = coolPinStg1;
	_heatPinStg1 = heatPinStg1;
	
}

void SegoACControl::begin()
{
	pinMode(_fanPin, OUTPUT);			//fan pin as digital output	
	digitalWrite(_fanPin, LOW);			//set fan pin as low
	pinMode(_coolPinStg1, OUTPUT);		//set stage 1 cool pin as digital output
	digitalWrite(_coolPinStg1, LOW);	//set stage 1 cool pin as low
	pinMode(_heatPinStg1, OUTPUT);		//set stage 1 heat pin as digital output
	digitalWrite(_heatPinStg1, LOW);	//set stage 1 heat pin as low
	
	/*should add more pins to control multiple heat and
	cool stages*/
	
	
	_protectACTimer = 0;				// TBD
	_protectACTimerDelta = 8000;		// TBD
	_ambientTempInterval = 10000;		//time to check thermometer
	_targetTemp = 25.00;				//target temperature set to 25
	_deltaTemp = 0.5;					//increment or decrement of target temperature set to 0.5
	_acProtection = false;				//set AC protection to OFF
	_acMode = 0;						//set AC mode to 0 = "OFF"
	_acModeMax = 2;						//COOL, HEAT and OFF
	_acMenu = 0;						//Initialize menu in 0
	_acMenuMax = 2;						//3 settings, MODE, SETTINGS and EXIT

}

void SegoACControl::setProtectACTimerDelta(unsigned int protectACTimerDelta)
{
	_protectACTimerDelta = protectACTimerDelta;
}

unsigned long SegoACControl::getProtectACTimerDelta()
{
	return _protectACTimerDelta;
}

void SegoACControl::setProtectACTimer(unsigned int protectACTimer)
{
	_protectACTimer = protectACTimer;
}

unsigned long SegoACControl::getProtectACTimer()
{
	return _protectACTimer;
}

void SegoACControl::setDeltaTemp(float deltaTemp)
{
	_deltaTemp = deltaTemp;
}

float SegoACControl::getDeltaTemp()
{
	return _deltaTemp;
}

void SegoACControl::setAmbientTemp(float ambientTemp)
{
	_ambientTemp = ambientTemp;
}

float SegoACControl::getAmbientTemp()
{
	return _ambientTemp;
}

void SegoACControl::setAmbientTempInterval(unsigned long ambientTempInterval)
{
	_ambientTempInterval = ambientTempInterval;
}

unsigned long SegoACControl::getAmbientTempInterval()
{
	return _ambientTempInterval;
}

void SegoACControl::setTargetTemp(float targetTemp)
{
	_targetTemp = targetTemp;
}

float SegoACControl::getTargetTemp()
{
	return _targetTemp;
}

void SegoACControl::setACMode(int acMode)
{
	if (acMode > _acModeMax)
	{
		acMode = 0;
	}
	else if (acMode < 0)
	{
		acMode = _acModeMax;
	}
	_acMode = acMode;
}

int SegoACControl::getACMode()
{
	
	return _acMode;
}

void SegoACControl::activateACMode()
{
	//////////////////////////////// Off
	if (_acMode == 0)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
			
	}
	//////////////////////////////// Cooling
	else if (_acMode == 1 && _ambientTemp > _targetTemp + _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,HIGH);
		digitalWrite(_coolPinStg1,HIGH);
		digitalWrite(_heatPinStg1,LOW);
		
		
	}else if (_acMode == 1 && _ambientTemp > _targetTemp + _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		
	}else if (_acMode == 1 && _ambientTemp <= _targetTemp + _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		
	}
	///////////////////////////////// Heating
	else if (_acMode == 2 && _ambientTemp < _targetTemp - _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,HIGH);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,HIGH);		
		
	}else if (_acMode == 2 && _ambientTemp < _targetTemp - _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		
	}else if (_acMode == 2 && _ambientTemp >= _targetTemp - _deltaTemp && _acProtection == false)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		
	}
}

String SegoACControl::stringACMode()
{
	switch (_acMode)
	{
		case 0:
			_stringACMode = "OFF";
			break;
		case 1:
			_stringACMode = "COOL";
			break;
		case 2:
			_stringACMode = "HEAT";
			break;	
	}
	return _stringACMode;
}
