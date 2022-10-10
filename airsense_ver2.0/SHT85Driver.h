#pragma once
#if(defined(PIN_SCL_GPIO) && defined(PIN_SDA_GPIO))

#include "SHTSensor.h"
#include "config.h"


SHTSensor sht(SHTSensor::SHT3X);

uint32_t lastReadSHT 			= 0;
float 	 TFT_temperature_C 		= 0;
float 	 TFT_temperature_F		= 0;
float 	 TFT_humidity_percent 	= 0;


/**
 * @brief	Doc gia tri cua SHT
 *
 * @return  None
 */
void SHT_getData()
{
	if (millis() - lastReadSHT > 5000)
	{
		float SHT_temperature = 0;
		float SHT_humidity = 0;
#ifdef	DEBUG_SERIAL
		Serial.print("temperature: ");
		Serial.println(temperature_calibInt_u16);
		Serial.print("humi: ");
		Serial.println(humidity_calibInt_u16);
#endif
		if (sht.readSample())
		{
		SHT_temperature = sht.getTemperature() +  temperature_calibInt_u16;
		SHT_humidity 	= sht.getHumidity()	   +  humidity_calibInt_u16;

#ifdef	DEBUG_SERIAL
		Serial.println("T = "+String(SHT_temperature)+"  "+"H = "+String(SHT_humidity));
#endif
		} else {
#ifdef	DEBUG_SERIAL
		Serial.print("SHT85 Error in readSample()\n");   
#endif
		}

		if(SHT_temperature>0 && SHT_humidity>0 && SHT_temperature<100 && SHT_humidity<100){
			TFT_temperature_C = SHT_temperature ;
			TFT_humidity_percent = SHT_humidity ;
		} else
		{
			TFT_temperature_C = 0;
			TFT_humidity_percent = 0;     
		}
		TFT_temperature_F = SHT_temperature + 273;
		lastReadSHT = millis(); 
	}
}


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
	} else 
	{
#ifdef	DEBUG_SERIAL
		Serial.print("SHT init(): failed\n");
#endif
	}
}


#endif
