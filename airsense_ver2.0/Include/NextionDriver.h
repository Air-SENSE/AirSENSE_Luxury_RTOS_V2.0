/************************************************************
**    TFT_o3_ppb(int)  TFT_o3_ppm(float)  TFT_o3_ug(int)  **    
**    min_o3_ppb(int)  min_o3_ppm(float)  min_o3_ug(int)  **
**    max_o3_ppb(int)  max_o3_ppm(float)  max_o3_ug(int)  **
**    TFT_temp(uint8_t)            TFT_wifi               **
**    TFT_humi(uint8_t)            TFT_SDcard             **
**    TFT_temp_F(uint16_t)         Rx: pin 33             **
**    TFT_pm25(int)                Tx: pin 25             **
**    TFT_pm10(int)                                       **
**    TFT_time(string)                                    **
*************************************************************/

#ifndef _NEXTIONDRIVER_H_
#define _NEXTIONDRIVER_H_

#pragma once 
#include <iostream>
#include <string>
#include "string.h"
#include <sstream>
#include "config.h"
#include "EasyNextionLibrary.h"
#include "Easy_Nextion_Library/src/EasyNextionLibrary.h"
#include <EEPROM.h>


EasyNex myNex(Serial);


void Screen_Init();
void Screen_GetDisplayData();
void Screen_SaveCalibData2SDcard();
void Screen_DisplayCalibData();
void Screen_DisplayData();


#endif
