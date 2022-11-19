/**
 * @file SHT85Sensor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "SHTSensor.h"
#include "DeviceManager.h"
#include "config.h"

#define ERROR_SHT_INIT_FAILED  			0x11
#define ERROR_SHT_READ_DATA_FAILED  	0x12


// khai bao doi tuong SHT
SHTSensor sht(SHTSensor::SHT3X);


/**
 * @brief	Khoi tao SHT Sensor
 *
 * @param[in]	wire: wire i2c
 * @return  ERROR_CODE
 */
ERROR_CODE SHT_init(TwoWire &wire, struct connectionStatus *_connectStatus)
{
#if(defined(PIN_SCL_GPIO) && defined(PIN_SDA_GPIO))
	if(sht.init(wire))
	{
		_connectStatus->shtSensor = status_et::CONNECTED;
		LOG_PRINT_INFORMATION("SHT init successfully");
		return ERROR_NONE;		// trang thai khoi tao sensor
	}
	_connectStatus->shtSensor = status_et::DISCONNECTED;
	LOG_PRINT_INFORMATION("SHT init failed");
	return ERROR_SHT_INIT_FAILED;			// trang thai khoi tao sensor
#else
	_connectStatus->shtSensor = status_et::DISCONNECTED;
	LOG_PRINT_INFORMATION("SCL and SDA pins not connect!");
	LOG_PRINT_INFORMATION("SHT init failed");
	return ERROR_SHT_INIT_FAILED;		
#endif
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
 * @return ERROR_CODE
 */
ERROR_CODE SHT_getData(struct connectionStatus *_connectionStatus, const uint16_t temperature_calibInt_u16, const uint16_t humidity_calibInt_u16, struct sensorData *_sensorData)
{
	float SHT_temperature;
	float SHT_humidity;
	if (sht.readSample())		// kiem tra tinh trang du lieu co the doc hay chua
	{
		_connectionStatus->shtSensor = status_et::CONNECTED;
		SHT_temperature = sht.getTemperature() +  temperature_calibInt_u16;				// lay du lieu nhiet do tu cam bien va calibration
		SHT_humidity 	= sht.getHumidity()	   +  humidity_calibInt_u16;				// lay du lieu do am tu cam bien va calibration
		LOG_PRINT_INFORMATION("Temperature = %f", SHT_temperature);
		LOG_PRINT_INFORMATION("Humidity = %f", SHT_humidity);
	}
	else
	{
		_connectionStatus->shtSensor = status_et::DISCONNECTED;
		LOG_PRINT_ERROR("Fail to read data.");
		return ERROR_SHT_READ_DATA_FAILED;
	}

	// kiem tra cac gia tri nhiet do do am co hop le
	if(SHT_temperature > 0 && SHT_humidity > 0 && SHT_temperature < 100 && SHT_humidity < 100)	
	{
		_sensorData->temperature = SHT_temperature;
		_sensorData->humidity 	 = SHT_humidity;
		LOG_PRINT_INFORMATION("SHT85 Sensor read data successfully!");
		return ERROR_NONE;
	} else
	{
		_sensorData->temperature = 0;
		_sensorData->humidity 	 = 0;
		LOG_PRINT_INFORMATION("SHT85 Sensor read data failed!");
		return ERROR_SHT_READ_DATA_FAILED;
	}
}


