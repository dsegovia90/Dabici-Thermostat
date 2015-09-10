#ifndef SegoPushButton_h
#define SegoPushButton_h


#include "Arduino.h"

class PushButton
{
	public:
	PushButton(int pinKnob);
	void begin();
	bool getStatus();

	private:
	int _pinKnob;
	bool _status;
};

#endif