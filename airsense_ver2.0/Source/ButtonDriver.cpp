#include <ButtonDriver.h>
#include "airsense_ver2.0/Include/ButtonDriver.h"
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
