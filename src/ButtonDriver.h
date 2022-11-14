/**
 * @file ButtonDriver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifndef _BUTTONDRIVER_H_
#define _BUTTONDRIVER_H_

#include "config.h"

#define BUTTON_NOT_PRESSED 1

#define ERROR_BUTTON_SET_STATUS_FAILED 0xb1

ERROR_CODE Button_init(uint8_t buttonPin, uint8_t mode, int defaultStatus)
{
	pinMode(buttonPin, mode);
	if (defaultStatus < 0)
	{
		return ERROR_BUTTON_SET_STATUS_FAILED;
	} else {
		defaultStatus = (defaultStatus > 1) ? 1 : defaultStatus;
		digitalWrite(buttonPin, defaultStatus);
		return ERROR_NONE;
	}
}

bool Button_isPressed(uint8_t buttonPin)
{
	return (digitalRead(buttonPin) != BUTTON_NOT_PRESSED) ? true : false;
}

#endif
