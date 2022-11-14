/**
 * @file DFRobotO3.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifdef O3_SENSOR_DF_ROBOT
#include "DFRobot_OzoneSensor.h"


#define COLLECT_NUMBER          20              // collect number, the collection range is 1-100
#define Ozone_IICAddress        OZONE_ADDRESS_3
/*   iic slave Address, The default is ADDRESS_3
	ADDRESS_0               0x70      // iic device address
	ADDRESS_1               0x71
	ADDRESS_2               0x72
	ADDRESS_3               0x73
*/

#define ERROR_DFROBOTO3_INIT_FAILED 0X81
#define ERROR_DFROBOTO3_READ_DATA_FAILED 0x82

DFRobot_OzoneSensor Ozone;


/**
 * @brief	Khoi tao module Ozone
 *
 * @return  ERROR_CODE
 */
ERROR_CODE DFRobotO3_init()
{
	if(Ozone.begin(Ozone_IICAddress)) 
	{
		LOG_PRINT_INFORMATION("I2c connect success !");
		Ozone.setModes(MEASURE_MODE_PASSIVE);
		return ERROR_NONE;
	}  else {
		LOG_PRINT_INFORMATION("I2c device number error !");
		return ERROR_DFROBOTO3_INIT_FAILED;
	}
}


/**
 * @brief	Luu tru gia tri vao bien TFF_o3_ppm, TFT_o3_ug
 *
 * @param[out] _sensorData_st pointer to struct data sensor
 * @return  ERROR_CODE
 */
ERROR_CODE DFRobotO3_getData(struct sensorData *_sensorData_st)
{
	uint32_t o3_ppb_temp = Ozone.readOzoneData(COLLECT_NUMBER);
	if (o3_ppb_temp > 0)
	{
		_sensorData_st->o3_ppb = o3_ppb_temp;
		// lay gia tri min  cua O3 theo don vi ppb
		_sensorData_st->o3_ppb_min  = (_sensorData_st->o3_ppb_min < _sensorData_st->o3_ppb ) ? _sensorData_st->o3_ppb_min : _sensorData_st->o3_ppb;
		// lay gia tri max  cua O3 theo don vi ppb
		_sensorData_st->o3_ppb_max  = (_sensorData_st->o3_ppb_max > _sensorData_st->o3_ppb ) ? _sensorData_st->o3_ppb_max : _sensorData_st->o3_ppb;

		_sensorData_st->o3_ppm 		= (_sensorData_st->o3_ppb)/1000;
		_sensorData_st->o3_ug  		= (_sensorData_st->o3_ppb)*1.96;
		_sensorData_st->o3_ug_min  	= (_sensorData_st->o3_ppb_min)*1.96;
		_sensorData_st->o3_ppm_min 	= (_sensorData_st->o3_ppb_min)/1000;
		_sensorData_st->o3_ug_max  	= (_sensorData_st->o3_ppb_max)*1.96;
		_sensorData_st->o3_ppm_max 	= (_sensorData_st->o3_ppb_max)/1000;
		
		LOG_PRINT_INFORMATION("DFRobotO3 read data successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("DFRobotO3 read data failed!");
		return ERROR_DFROBOTO3_READ_DATA_FAILED;
	}
}
#endif
