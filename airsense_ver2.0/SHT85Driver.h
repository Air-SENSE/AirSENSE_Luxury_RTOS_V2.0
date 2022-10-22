#pragma once
#if(defined(PIN_SCL_GPIO) && defined(PIN_SDA_GPIO))

#include "SHTSensor.h"
#include "config.h"

#define SHT85_GETDATA_PERIOD	 (uint32_t)5000

// khai bao doi tuong SHT
SHTSensor sht(SHTSensor::SHT3X);

// khai bao cac bien
float 	 TFT_temperature_C 		= 0;
float 	 TFT_temperature_F		= 0;
float 	 TFT_humidity_percent 	= 0;


/**
 * @brief	Khoi tao SHT Sensor
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


/**
 * @brief	Doc gia tri cua SHT
 *
 * @return  None
 */
void SHT_getData()
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
		SHT_temperature = sht.getTemperature() +  temperature_calibInt_u16;				// lay du lieu nhiet do tu cam bien va calibration
		SHT_humidity 	= sht.getHumidity()	   +  humidity_calibInt_u16;				// lay du lieu do am tu cam bien va calibration
#ifdef	DEBUG_SERIAL
	Serial.println("T = " + String(SHT_temperature) + "  " + "H = " + String(SHT_humidity));
#endif
	}

	// kiem tra cac gia tri nhiet do do am co hop le
	if(SHT_temperature > 0 && SHT_humidity > 0 && SHT_temperature < 100 && SHT_humidity < 100)	
	{
		TFT_temperature_C = SHT_temperature ;
		TFT_humidity_percent = SHT_humidity ;
	} else
	{
		TFT_temperature_C = 0;
		TFT_humidity_percent = 0;     
	}
	TFT_temperature_F = TFT_temperature_C + 273;
}





#endif
