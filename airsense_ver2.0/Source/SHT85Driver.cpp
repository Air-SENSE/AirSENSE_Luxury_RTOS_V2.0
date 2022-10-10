#include <airsense_ver2.0/Include/SHT85Driver.h>


/**
 * @brief	Khoi tao SHT
 *
 * @return  None
 */
void SHT_init()
{
	if (sht.init())
    {
#ifdef	DEBUG_SERIAL
		Serial.print("SHT init(): success\n");
#endif
	} else {
#ifdef	DEBUG_SERIAL
		Serial.print("SHT init(): failed\n");
#endif
	}
}


/**
 * @brief	Doc gia tri cua SHT
 *
 * @return  None
 */
void SHT_getData()
{
	if ( (millis() - lastReadSHT_u32 > SHT85_GETDATA_PEROID)
	{
		float SHT_temperature = 0;
		float SHT_humidity = 0;

#ifdef	DEBUG_SERIAL
		Serial.print("Temp: ");
		Serial.println(tempCalibInt);
		Serial.print("Humi: ");
		Serial.println(humiCalibInt);
#endif

		if (sht.readSample()) 
		{
			SHT_temperature = sht.getTemperature() + tempCalibInt;
			SHT_humidity = sht.getHumidity() + humiCalibInt;
#ifdef	DEBUG_SERIAL
			Serial.println("T = "+String(SHT_temp)+"  "+"H = "+String(SHT_humi));
#endif
		} else 
		{
#ifdef	DEBUG_SERIAL
			Serial.print("SHT85 Error in readSample()\n");
#endif
		}

		if((SHT_temperature > 0) && (SHT_humidity > 0) && (SHT_temperature < 100) && (SHT_humidity < 100))
		{
			TFT_temperature_C = SHT_temperature ;
			TFT_humidity_percent = SHT_humidity ;
		} else
		{
			TFT_temperature_C = 0;
			TFT_humidity_percent = 0;     
		}
		TFT_temperature_F = TFT_temperature_C + 273;
		lastReadSHT_u32 = millis();
	}
}


