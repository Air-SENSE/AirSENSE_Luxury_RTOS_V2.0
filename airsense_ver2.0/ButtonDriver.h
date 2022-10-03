#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <config.h>

#define BUTTON_PRESS_DURATION	(uint32_t)4000 	// millisecond

uint32_t lastPressButton = 0;

bool Button_isLongPressed()
{
	if (millis() - lastPressButton > BUTTON_PRESS_DURATION && digitalRead(PIN_BUTTON_1) == 0)
	{
		return true;
	}
	else if (digitalRead(PIN_BUTTON_1) == 1)
	{
		lastPressButton = millis();
	}
	return false;
}

#endif