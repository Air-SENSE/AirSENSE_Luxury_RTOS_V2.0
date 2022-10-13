#include "RTClib.h"
#include <ESP32Ping.h>

const char* remote_host_string = "www.google.com";
RTC_DS3231 realTime;
char  TFT_time_string[16];


/**
 * @brief	Khoi tao module thoi gian thuc
 *
 * @return  None
 */
void DS3231_init()
{
	realTime.begin();
	if (WiFi.status() == wl_status_t::WL_CONNECTED){
		if (Ping.ping(remote_host_string))
		{
			timeClient.update();
			uint32_t epochTime_u32 = timeClient.getEpochTime();
			realTime.adjust(DateTime(epochTime_u32));
			Serial.println("Updatetime DS3231.");
		}
	}
	
#ifdef	DEBUG_SERIAL
	Serial.println(realTime.now().toString("YYYY-MMM-DD,hh:mm:ss"));
#endif
}


/**
 * @brief	luu tru thoi gian thuc vao bien TFT_time_string
 *
 * @return  None
 */
bool DS3231_getData()
{
	if (realTime.now().isValid())
	{
		strcpy(TFT_time_string, realTime.now().toString("hh:mm DD-MMM-YY"));
#ifdef  DEBUG_SERIAL
	Serial.println(TFT_time_string);
#endif
		return true;
	} else {
#ifdef  DEBUG_SERIAL
	Serial.println("------ *** CAN'T GET TIME *** -----");
#endif
		return false;
	}
}
