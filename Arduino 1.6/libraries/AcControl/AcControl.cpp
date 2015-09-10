#include "AcControl.h"

AcControl::AcControl(int fanPin, int coolPinStg1, int heatPinStg1)
{
	_fanPin = fanPin;
	_coolPinStg1 = coolPinStg1;
	_heatPinStg1 = heatPinStg1;
	
}

void AcControl::begin(int acMode,float ambientTemp,float targetTemp,float deltaTemp,unsigned long ambientTempInterval)
{
	_acMode=acMode;
    	_ambientTemp = ambientTemp;
	_targetTemp = targetTemp;
	_deltaTemp = deltaTemp;
	_ambientTempInterval = ambientTempInterval;
	_acProtection = false;
	pinMode(_fanPin, OUTPUT);
	digitalWrite(_fanPin, LOW);
	pinMode(_coolPinStg1, OUTPUT);
	digitalWrite(_coolPinStg1, LOW);
	pinMode(_heatPinStg1, OUTPUT);
	digitalWrite(_heatPinStg1, LOW);
	
}

void AcControl::setDeltaTemp(float deltaTemp)
{
	_deltaTemp = deltaTemp;
}


void AcControl::setAmbientTemp(float ambientTemp)
{
	_ambientTemp = ambientTemp;
}


void AcControl::setAmbientTempInterval(unsigned long ambientTempInterval)
{
	_ambientTempInterval = ambientTempInterval;
}


void AcControl::setTargetTemp(float targetTemp)
{
	_targetTemp = targetTemp;
}


void AcControl::setACMode(int acMode)
{
	_acMode = acMode;
}

void AcControl::setAcProtection(bool acProtection)
{
	_acProtection = acProtection;
}

boolean AcControl::getAcProtection()
{
	return _acProtection;
}

void AcControl::activateACMode()
{
	//////////////////////////////// Off
	if (_acMode == 0)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		
	}
	//////////////////////////////// Cooling
	else if (_acMode == 1 && _ambientTemp > _targetTemp + _deltaTemp /*&& _acProtection == false*/)
	{
		digitalWrite(_fanPin,HIGH);
		digitalWrite(_coolPinStg1,HIGH);
		digitalWrite(_heatPinStg1,LOW);
		
		
	}else if (_acMode == 1 && _ambientTemp <= _targetTemp + _deltaTemp /*&& _acProtection == false*/)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		_acProtection=true;//use getAcProtection(); to check and define in main program a protection sequence
		
	}
	///////////////////////////////// Heating
	else if (_acMode == 2 && _ambientTemp < _targetTemp - _deltaTemp /*&& _acProtection == false*/)
	{
		digitalWrite(_fanPin,HIGH);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,HIGH);
		
	}else if (_acMode == 2 && _ambientTemp >= _targetTemp - _deltaTemp /*&& _acProtection == false*/)
	{
		digitalWrite(_fanPin,LOW);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
		_acProtection=true;//use getAcProtection(); to check and define in main program a protection sequence
		
}
//////////////////////////////////////Fan On
	else if (_acMode == 3 &&_acProtection == false)
	{
		digitalWrite(_fanPin,HIGH);
		digitalWrite(_coolPinStg1,LOW);
		digitalWrite(_heatPinStg1,LOW);
	
	}
}