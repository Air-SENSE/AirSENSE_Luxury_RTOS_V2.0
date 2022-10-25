#pragma once
#if(defined(PIN_SCL_GPIO) && defined(PIN_SDA_GPIO))

#include "SHTSensor.h"
#include "config.h"

#define ERROR_NONE				0x00
#define ERROR_SHT_INIT_SUCCESS	ERROR_NONE 	
#define ERROR_SHT_INIT_FAILED  	0x11
#define ERROR_SHT_READ_FAILED  	0x12



// khai bao doi tuong SHT
SHTSensor sht(SHTSensor::SHT3X);

// khai bao cac bien
float 	 TFT_temperature_C 		= 0;
float 	 TFT_temperature_F		= 0;
float 	 TFT_humidity_percent 	= 0;


/**
 * @brief	Khoi tao SHT Sensor
 *
 * @return  uint16_t : trang thai khoi tao SHT sensor
 */
uint16_t SHT_init()
{
	if (sht.init()) 
	{
#ifdef	DEBUG_SERIAL
		LOG_PRINT_NOTIFICATION("SHT init(): success\n");
#endif
		return ERROR_SHT_INIT_SUCCESS;		// trang thai khoi tao sensor
	} else 
	{
#ifdef	DEBUG_SERIAL
		LOG_PRINT_NOTIFICATION("SHT init(): failed\n");
#endif
		return ERROR_SHT_READ_FAILED;			// trang thai khoi tao sensor
	}
}


/**
 * @brief	Doc gia tri cua SHT
 *
 * @param[in]  temperature_calibInt_16	: gia tri calibration nhiet do
 * @param[in]  humidity_calibInt_u16	: gia tri calibration do am
 * 
 * @param[out] temperature 	: gia tri nhiet do tu cam bien
 * @param[out] humidity 	: gia tri do am tu cam bien
 * 
 * @return  uint16_t : ERROR CODE
 */
uint16_t SHT_getData(const uint16_t temperature_calibInt_u16,
					 const uint16_t humidity_calibInt_u16,
					 FLOAT_32_16* 	temperature,
					 FLOAT_32_16* 	humidity)
{
	float SHT_temperature;
	float SHT_humidity;
	if (sht.readSample())		// kiem tra tinh trang du lieu co the doc hay chua
	{
		SHT_temperature = sht.getTemperature() +  temperature_calibInt_u16;				// lay du lieu nhiet do tu cam bien va calibration
		SHT_humidity 	= sht.getHumidity()	   +  humidity_calibInt_u16;				// lay du lieu do am tu cam bien va calibration
		LOG_PRINT_INFORMATION("Temperature = %f", SHT_temperature);
		LOG_PRINT_INFORMATION("Humidity = %f", SHT_humidity);
	}
	else
	{
		LOG_PRINT_ERROR("Fail to read data.");
		return ERROR_SHT_READ_FAILED;
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
	TFT_temperature_K = TFT_temperature_C + 273;
	return ERROR_NONE;
}

#endif
