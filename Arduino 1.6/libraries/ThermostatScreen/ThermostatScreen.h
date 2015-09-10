#ifndef ThermostatScreen_h
#define ThermostatScreen_h


#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"



#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

// These are the pins used for the UNO
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
extern Adafruit_ILI9340 tft;

class ThermostatScreen
{
	public:
	ThermostatScreen(int x, int y, int textSize, uint16_t color);
	
	int toPrintInt;
	String toPrintString;
	void begin();
	
	void print(float toPrintFloat);
	void print(String toPrintString);
	
	void eraseFloat();
	void eraseString();
	
	void setXcord(int x);
	void setYcord(int y);
	

	private:
	int _x;
	int _y;
	int _textSize;
	uint16_t _color;
	float _toPrintFloat;
	String _toPrintString;
	float _toEraseFloat;
	String _toEraseString;
	bool _flash;
	unsigned long timer;
};

#endif