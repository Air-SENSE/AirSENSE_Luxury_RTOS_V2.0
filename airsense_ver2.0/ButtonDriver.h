#pragma once
#ifndef _BUTTONDRIVER_H_
#define _BUTTONDRIVER_H_

#if(defined(PIN_BUTTON_1))

#include "config.h"

#define BUTTON_PRESS_DURATION	(uint32_t)4000 	// millisecond

uint32_t lastPressButton = 0;


bool Button_isLongPressed()
{
	if (millis() - lastPressButton > BUTTON_PRESS_DURATION && digitalRead(PIN_BUTTON_1) == 0) 			// kiem tra  thoi gian bam nut
	{
		return true;
	}
	else if (digitalRead(PIN_BUTTON_1) == 1)
	{
		lastPressButton = millis();				// lấy giời gian nếu bút nhấn vẫn được nhấn
	}
	return false;
}


#endif
#endif