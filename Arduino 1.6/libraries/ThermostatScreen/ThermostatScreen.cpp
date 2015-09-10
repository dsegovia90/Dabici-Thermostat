#include "ThermostatScreen.h"

extern Adafruit_ILI9340 tft(_cs, _dc, _rst);

ThermostatScreen::ThermostatScreen(int x, int y, int textSize, uint16_t color)
{
	_x = x;
	_y = y;
	_textSize = textSize;
	_color = color;
	
}
void ThermostatScreen::begin()
{
	
}



void ThermostatScreen::print(float toPrintFloat)
{
	_toPrintFloat = toPrintFloat;
	eraseFloat();
	tft.setCursor(_x,_y);
	tft.setTextSize(_textSize);
	tft.setTextColor(_color);

	tft.print(_toPrintFloat);
	_toEraseFloat = _toPrintFloat;
}
void ThermostatScreen::print(String toPrintString)
{
	_toPrintString = toPrintString;
	eraseString();
	tft.setCursor(_x,_y);
	tft.setTextSize(_textSize);
	tft.setTextColor(_color);
	
	tft.print(_toPrintString);
	_toEraseString = _toPrintString;
}
void ThermostatScreen::eraseFloat()
{
	tft.setCursor(_x,_y);
	tft.setTextSize(_textSize);
	tft.setTextColor(ILI9340_BLACK);
	tft.print(_toEraseFloat);
}
void ThermostatScreen::eraseString()
{
	tft.setCursor(_x,_y);
	tft.setTextSize(_textSize);
	tft.setTextColor(ILI9340_BLACK);
	tft.print(_toEraseString);
}

