#pragma once
#include "RTClib.h"
#include <ESP32Ping.h>
#include "MQTTConnection.h"
#include "log.h"


/* DS3231 init status */
#define DS3231_INIT_SUCCESS			0x70
#define DS3231_INIT_UNSUCCESS		0x71
#define DS3231_GETDATA_SUCCESS		0x72
#define DS3231_GETDATA_UNSUCCESS	0x73

const char *remote_host_string = "www.google.com";
RTC_DS3231 realTime;

/**
 * @brief	Khoi tao module thoi gian thuc
 *
 * @return  DS3231_INITSUCCESS		0x70: Khoi tao DS3231 thanh cong
 * @return  DS3231_INITUNSUCCESS	0x71: Khoi tao DS3231 khong thanh cong
 */
uint32_t DS3231_init()
{
	realTime.begin();									// khoi dong module RTC
	if (WiFi.status() == wl_status_t::WL_CONNECTED)		// kiem tra co ket noi wifi
	{
		if (Ping.ping(remote_host_string))				// kiem tra ping duong dan "www.google.com"
		{
			// cap nhat thoi gian cho RTC
			timeClient.update();				
			uint32_t epochTime_u32 = timeClient.getEpochTime();
			realTime.adjust(DateTime(epochTime_u32));			// Set the date and flip the Oscillator Stop Flag
			LOG_PRINT_NOTIFICATION("Updatetime DS3231....");
			return DS3231_INIT_SUCCESS;					// Khoi tao DS3231 thanh cong
		}
	}
	return DS3231_INIT_UNSUCCESS;						// Khoi tao DS3231 khong thanh cong
	//LOG_PRINT_NONE(realTime.now().toString("YYYY-MMM-DD hh:mm:ss"));
//#ifdef	DEBUG_SERIAL
//	Serial.println(realTime.now().toString("YYYY-MMM-DD hh:mm:ss")); 		// in thoi gian hien tai ra Serial
//#endif
}


/**
 * @brief	luu tru thoi gian thuc vao bien TFT_time_string
 *
 * @return DS3231_GETDATA_SUCCESS	0x72 : neu lay duoc thoi gian hop le
 * @return DS3231_GETDATA_UNSUCCESS	0x73 :neu thoi gian khong hop le
 */
bool DS3231_getData()
{
	if (realTime.now().isValid())		// kiem tra thoi gian co hop le
	{
#ifdef  DEBUG_SERIAL
	LOG_PRINT_NOTIFICATION(getDateTime("hh:mm DD-MMM-YY"));		// in thoi gian hien tai ra Serial
#endif
		return DS3231_GETDATA_SUCCESS;
	} else {
#ifdef  DEBUG_SERIAL
	LOG_PRINT_ERROR("------ *** CAN'T GET TIME *** -----");
#endif
		return DS3231_GETDATA_UNSUCCESS;
	}
}