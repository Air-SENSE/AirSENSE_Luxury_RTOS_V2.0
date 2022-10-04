#include <SHT85Driver.h>


/**
 * @brief	Khoi tao SHT
 *
 * @return  None
 */
void SHT_Init()
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
	if ( (millis() - lastReadSHT > 5000)
	{
		float SHT_temp = 0;
		float SHT_humi = 0;
#ifdef	DEBUG_SERIAL
		Serial.print("Temp: ");
		Serial.println(tempCalibInt);
		Serial.print("Humi: ");
		Serial.println(humiCalibInt);
#endif
		if (sht.readSample()) 
		{
			SHT_temp = sht.getTemperature() + tempCalibInt;
			SHT_humi = sht.getHumidity() + humiCalibInt;
#ifdef	DEBUG_SERIAL
			Serial.println("T = "+String(SHT_temp)+"  "+"H = "+String(SHT_humi));
#endif
		} else 
		{
#ifdef	DEBUG_SERIAL
			Serial.print("SHT85 Error in readSample()\n");
#endif
		}

		if((SHT_temp > 0) && (SHT_humi > 0) && (SHT_temp < 100) && (SHT_humi < 100))
		{
			TFT_temperature_C = (uint32_t)SHT_temp ;
			TFT_humidity_percent = (uint32_t)SHT_humi ;
			TFT_temperature_F = (uint32_t)SHT_temp + 273;
		} else
		{
			TFT_temperature = 0;
			TFT_humidity_percent = 0;     
		}
	}
}


