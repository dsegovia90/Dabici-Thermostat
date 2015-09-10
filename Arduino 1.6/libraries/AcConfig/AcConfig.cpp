#include "AcConfig.h"


void AcConfig::begin()
{
	_deltaTemp = .5;
	_stepTemp = .5;
	_ambientTempInterval=10000;
	_targetTemp=25;
	_acMode=0;
	_maxTemp=30;
	_minTemp=18;
}

void AcConfig::setDeltaTemp(float deltaTemp)
{
	_deltaTemp = deltaTemp;
}

float AcConfig::getDeltaTemp()
{
	return _deltaTemp;
}

void AcConfig::setStepTemp(float stepTemp)
{
	_stepTemp=stepTemp;
}

float AcConfig::getStepTemp()
{
	return _stepTemp;
}

void AcConfig::setMaxTemp(float maxTemp)
{
	_maxTemp=maxTemp;
}
float AcConfig::getMaxTemp()
{
	return _maxTemp;
}

void AcConfig::setMinTemp(float minTemp)
{
	_minTemp=minTemp;
}

float AcConfig::getMinTemp()
{
	return _minTemp;
}

void AcConfig::setAmbientTemp(float ambientTemp)
{
	_ambientTemp = ambientTemp;
}

float AcConfig::getAmbientTemp()
{
	return _ambientTemp;
}

void AcConfig::setAmbientTempInterval(unsigned long ambientTempInterval)
{
	_ambientTempInterval = ambientTempInterval;
}

unsigned long AcConfig::getAmbientTempInterval()
{
	return _ambientTempInterval;
}

void AcConfig::setTargetTemp(float targetTemp)
{
	_targetTemp = targetTemp;
}

float AcConfig::getTargetTemp()
{
	return _targetTemp;
}

void AcConfig::setACMode(int acMode)
{
	_acMode = acMode;
}

int AcConfig::getACMode()
{
	
	return _acMode;
}