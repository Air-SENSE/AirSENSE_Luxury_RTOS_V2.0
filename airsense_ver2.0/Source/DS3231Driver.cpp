#include <airsense_ver2.0/Include/DS3231Driver.h>

/**
 * @brief	Khoi tao module thoi gian thuc
 *
 * @return  None
 */
void DS3231_init()
{ 
	rtc.begin();
	if (WiFi.status() == wl_status_t::WL_CONNECTED)
	{
		if (Ping.ping(remote_host_string))
		{
			timeClient.update();
			uint32_t Time_epoch_u32 = timeClient.getEpochTime();
			rtc.adjust(DateTime(Time_epoch_u32));
			Serial.println("Updatetime DS3231.");
		}
	}
	DateTime now = rtc.now();
#ifdef	DEBUG_SERIAL
	Serial.print(now.year(), DEC);
	Serial.print('/');
	Serial.print(now.month(), DEC);
	Serial.print('/');
	Serial.print(now.day(), DEC);
	Serial.print(" ");
	Serial.print(now.hour(), DEC);
	Serial.print(':');
	Serial.print(now.minute(), DEC);
	Serial.print(':');
	Serial.print(now.second(), DEC);
	Serial.println();
#endif
}


/**
 * @brief	luu tru thoi gian thuc vao bien TFT_time_string
 *
 * @return  None
 */
void DS3231_getData()
{
	DateTime now = rtc.now();
	if(now.hour()<10 && now.minute()<10)
	{
		sprintf(TFT_time_string,"0%d:0%d  %d/%d/%d",int(now.hour()),int(now.minute()),int(now.day()),int(now.month()),int(now.year()%2000));
	}

	if(now.hour()>=10 && now.minute()<10)
	{
		sprintf(TFT_time_string,"%d:0%d  %d/%d/%d",int(now.hour()),int(now.minute()),int(now.day()),int(now.month()),int(now.year()%2000));
	} 

	if(now.hour()<10 && now.minute()>=10)
	{
		sprintf(TFT_time_string,"0%d:%d  %d/%d/%d",int(now.hour()),int(now.minute()),int(now.day()),int(now.month()),int(now.year()%2000));
	} 

	if(now.hour()>=10 && now.minute()>=10)
	{
		sprintf(TFT_time_string,"%d:%d  %d/%d/%d",int(now.hour()),int(now.minute()),int(now.day()),int(now.month()),int(now.year()%2000));
	} 
	
#ifdef  DEBUG_SERIAL
	Serial.println(TFT_time_string);
#endif
}