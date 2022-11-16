/**
 * @file TFLP01Driver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#if(defined(PIN_TX_TFLP01) && defined(PIN_RX_TFLP01))

#include "config.h"
#include "DeviceManager.h"

#define ERROR_TFLP01_INIT_FAILED 0x21
#define ERROR_TFLP01_READ_DATA_FAILED 0x22


/**
 * @brief  Return CRC trong data response của TF-LP-01
 *
 * @return  None
 */
//uint16_t crc16_modbus(uint8_t *modbusdata , uint16_t Length) 
// {
// 	uint16_t crc16 = 0xFFFF;
// 	for (uint16_t i = 0; i < Length; i++)
// 	{
// 		crc16 ^= modbusdata[i];
// 		for (uint16_t j = 0; j < 8; j++)
// 		{
// 			if ((crc16 & 0x01) == 1) crc16 = (crc16 >> 1) ^ 0xA001; 
// 			else  crc16 >>= 1;
// 		}
// 	}
// 	return crc16;
// }


/**
 * @brief initialized TFLP01 Sensor
 *
 * @return  ERROR_CODE
 */
ERROR_CODE TFLP01_init(HardwareSerial& _stream, uint32_t baudRate_u32, struct connectionStatus *connectionStatus_st)
{
	_stream.begin(baudRate_u32);
	if (_stream.available() > 0)
	{
		connectionStatus_st->tflp01Sensor = status_et::CONNECTED;
		LOG_PRINT_INFORMATION("TFLP01 Sensor initialized successfully!");
		return ERROR_NONE;
	} else {
		connectionStatus_st->tflp01Sensor = status_et::DISCONNECTED;
		LOG_PRINT_INFORMATION("TFLP01 Sensor initialized failed!");
		return ERROR_TFLP01_INIT_FAILED;
	}
}


/**
 * @brief Send a request read raw data to TFLP01 Sensor
 * 
 * @param[in] dataArray pointer to array data
 * 
 * @return ERROR_CODE 
 */
ERROR_CODE TFLP01_readRawData(uint8_t *dataArray, size_t lenght)
{
	LOG_PRINT_NOTIFICATION("TFLP01 Sensor send read request...");
	lenght = (lenght > 17) ? 17 : lenght;
	if (TFLP01_SERIAL_PORT.available() > 0) 
	{
		LOG_PRINT_INFORMATION("Check: ");
		for(uint8_t i = 0; i < lenght; i++)
		{
			*(dataArray+i) = TFLP01_SERIAL_PORT.read();			// doc tung byte du lieu tu TFLP01 qua Serial2
			LOG_PRINT_INFORMATION("%d: %d.", i, *(dataArray+i));
		}
		return ERROR_NONE;
	} else {
		return ERROR_TFLP01_READ_DATA_FAILED;
	}
}

/**
 * @brief Read data from TFLP01 sensor
 *
 * @param[in] _calibData: pointer to calibration data struct
 * @param[out] _sensorData_st pointer to struct 
 * 
 * @return  ERROR_CODE
 */
ERROR_CODE TFLP01_getData(struct sensorData *_sensorData_st, struct calibData *_calibData)
{
	uint8_t TFLP01_data_arr[17] = {0};
	// lay du lieu tam thoi (chua co datasheet)
	if(TFLP01_readRawData(TFLP01_data_arr, 16) == ERROR_NONE)
	{
		_sensorData_st->pm1_u32  = TFLP01_data_arr[9]  + TFLP01_data_arr[10]*256 + _calibData->pm1_calibInt_u32;
		_sensorData_st->pm25_u32 = TFLP01_data_arr[11] + TFLP01_data_arr[12]*256 + _calibData->pm25_calibInt_u32;
		_sensorData_st->pm10_u32 = TFLP01_data_arr[13] + TFLP01_data_arr[14]*256 + _calibData->pm10_calibInt_u32;

		if(_sensorData_st->pm25_u32 != 255)
		{
			_sensorData_st->pm25_max_u32 = (_sensorData_st->pm25_u32 > _sensorData_st->pm25_max_u32) ? _sensorData_st->pm25_u32 : _sensorData_st->pm25_max_u32;
			_sensorData_st->pm25_min_u32 = (_sensorData_st->pm25_u32 < _sensorData_st->pm25_min_u32) ? _sensorData_st->pm25_u32 : _sensorData_st->pm25_min_u32;
		}

		LOG_PRINT_INFORMATION("PM1.0: %u", _sensorData_st->pm1_u32 );
		LOG_PRINT_INFORMATION("PM2.5: %u", _sensorData_st->pm25_u32 );
		LOG_PRINT_INFORMATION("PM10.0: %u", _sensorData_st->pm10_u32 );
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("TFLP01 Sensor read data failed!");
		return ERROR_TFLP01_READ_DATA_FAILED;
	}
}

#endif
