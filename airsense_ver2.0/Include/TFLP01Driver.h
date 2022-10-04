#pragma once
#ifndef _TFLP01DRIVER_H_
#define _TFLP01DRIVER_H_

#ifdef PIN_TX_TFLP01 && PIN_RX_TFLP01

#include "config.h"

uint32_t  TFLP01_pm10sum 	= 0;
uint32_t  TFLP01_pm25sum 	= 0;
uint32_t  TFLP01_pm1sum 	= 0;
int 	  dataTFLP01count 	= 0;
bool 	  TFLP01_read 		= false;
uint32_t  TFT_pm1			= 0;
uint32_t  TFT_pm25			= 0;
uint32_t  TFT_pm10			= 0;
uint32_t  max_pm25			= 0;
uint32_t  min_pm25			= 1000;


uint16_t crc16_modbus(uint8_t * , uint16_t);
void TFLP01_Init();
void TFLP01_getData();


#endif
#endif
