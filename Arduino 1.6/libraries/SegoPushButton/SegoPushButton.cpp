#include "SegoPushButton.h"

PushButton::PushButton(int pinKnob)
{
	_pinKnob = pinKnob;
}
void PushButton::begin()
{
	pinMode(_pinKnob,INPUT_PULLUP);
}
bool PushButton::getStatus()
{
	_status = digitalRead(_pinKnob);
	return _status;
}