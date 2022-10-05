#pragma once
#ifndef _SHT85DRIVER_H_
#define _SHT85DRIVER_H_

#ifdef PIN_SCL_GPIO && PIN_SDA_GPIO

#include "SHTSensor.h"
#include "config.h"

SHTSensor sht(SHTSensor::SHT3X);

uint32_t 	lastReadSHT_u32 	    = 0; 	// thoi diem cuoi cung doc cam bien
uint32_t    TFT_temperature_C 		= 0;
uint32_t    TFT_temperature_F		= 0;
uint32_t    TFT_humidity_percent	= 0;


void SHT_init();
void SHT_getData();


#endif
#endif
