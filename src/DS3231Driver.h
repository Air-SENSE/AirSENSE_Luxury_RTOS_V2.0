/**
 * @file DS3231Driver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include "RTClib.h"
#include <ESP32Ping.h>
#include "MQTTConnection.h"
#include "log.h"

const char* remote_host_string = "www.google.com";
RTC_DS3231 realTime;


#define ERROR_RTC_INIT_FAILED 0x71
#define ERROR_RTC_GET_TIME_CURRENT_FAILED 0x72
#define ERROR_RTC_UPDATE_TIME_FAILED 0x73
#define ERROR_RTC_GET_STRING_DATETIME_FAILED 0x74



/**
 * @brief Initialize RTC module
 * 
 * @param[in]  _realTime: RTC_DS3231 object
 * @param[in]  _timeClient: NTPClient object
 * @param[in]  _wire: Wire I2c
 * @param[in]  _connecctionStatus: struct status connection
 * @return ERROR_CODE
 */
ERROR_CODE DS3231_init(RTC_DS3231& _realTime, NTPClient&  _timeClient, TwoWire &_wire, struct connectionStatus _connectionStatus)
{
	if (_realTime.begin(&_wire))			// khoi dong module RTC
	{
		LOG_PRINT_INFORMATION("RTC module initialized successfully!");
		if (_connectionStatus.wifiStatus == status_et::CONNECTED)		// kiem tra co ket noi wifi
		{
			if (Ping.ping(remote_host_string))				// kiem tra ping duong dan "www.google.com"
			{
				_timeClient.update();						// cap nhat thoi gian cho RTC	
				uint32_t epochTime_u32 = _timeClient.getEpochTime();
				_realTime.adjust(DateTime(epochTime_u32));			// Set the date and flip the Oscillator Stop Flag
				LOG_PRINT_INFORMATION("Updatetime DS3231....");
				LOG_PRINT_INFORMATION("Updatetime success. Current time: %s.", _realTime.now().toString("YYYY-MMM-DD hh:mm:ss"));
			}
			return ERROR_NONE;
		} else {
			LOG_PRINT_INFORMATION("Wifi is disconnect.");
			LOG_PRINT_INFORMATION("Updatetime failed.");
			return ERROR_RTC_UPDATE_TIME_FAILED;
		}

	} else {
		LOG_PRINT_INFORMATION("RTC module initialized failed!");
		return ERROR_RTC_INIT_FAILED;
	}
}



/**
 * @brief	Get string datetime
 *
 * @param[in]  _realTime: RTC_DS3231 object
 * @param[in]  _format: format of string datetime. Example "hh:mm DD-MMM-YY"
 * @param[out] _dateTime_string: string datetime return
 * @return ERROR_CODE
 */
ERROR_CODE DS3231_getStringDateTime(RTC_DS3231 _realTime, char *_format, char *_dateTime_string)
{
	if (_realTime.now().isValid())		// kiem tra thoi gian co hop le
	{
		strncpy(_dateTime_string, (const char*)(_realTime.now().toString(_format)), strlen(_format));		// in thoi gian hien tai ra Serial
		LOG_PRINT_INFORMATION("Get DateTime successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("Get DateTime failed.");
		return ERROR_RTC_GET_STRING_DATETIME_FAILED;
	}
}
