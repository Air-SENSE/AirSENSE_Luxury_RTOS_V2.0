#pragma once
#ifndef _BUTTONDRIVER_H_
#define _BUTTONDRIVER_H_

#ifdef PIN_BUTTON_1 || PIN_BUTTON_2

#define BUTTON_PRESS_DURATION	(uint32_t)4000 	// millisecond

uint32_t lastPressButton = 0;

bool Button_isLongPressed();

#endif

#endif