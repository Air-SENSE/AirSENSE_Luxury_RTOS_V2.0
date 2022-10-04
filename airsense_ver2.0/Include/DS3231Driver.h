#pragma once
#ifndef _DS3231DRIVER_H_
#define _DS3231DRIVER_H_

#include "RTClib.h"
#include <ESP32Ping.h>

const char* remote_host = "www.google.com";
RTC_DS3231 rtc;
char  TFT_time[16];


void DS3231_Init();
void DS3231_getData();


#endif