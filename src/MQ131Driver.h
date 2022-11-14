/**
 * @file MQ131Driver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#ifdef O3_SENSOR_MQ131
#include <MQ131.h>

#define ERROR_MQ131_INIT_FAILED		 0x61
#define ERROR_MQ131_READ_DATA_FAILED 0X62


ERROR_CODE isTimeToReadData();

/**
 * @brief	Khoi tao module MQ131
 *
 * @return  None
 */
ERROR_CODE MQ131Sensor_init()
{
	MQ131.begin(PIN_MQ131_POWER, PIN_MQ131_SENSOR, MQ131Model::LOW_CONCENTRATION, MQ131_DEFAULT_RL, &DEBUG_SERIAL_PORT);  	// khoi dong sensor  MQ131
	LOG_PRINT_INFORMATION("Calibration in progress...");
	MQ131.calibrate();		//Calibrate the basic values
	LOG_PRINT_INFORMATION("Calibration done!");
	LOG_PRINT_INFORMATION("R0 = %f Ohms.", MQ131.getR0());
	LOG_PRINT_INFORMATION("Time to heat = %lds.", MQ131.getTimeToRead());
	if (isTimeToReadData() == ERROR_NONE)
	{
		return ERROR_NONE;
	} else {
		return ERROR_MQ131_INIT_FAILED;
	}
}


ERROR_CODE isTimeToReadData()
{
	uint32_t o3_ppmTemp_u32 = MQ131.getO3(MQ131Unit::PPM);
	if ((10 < o3_ppmTemp_u32) && (o3_ppmTemp_u32 < 1000))
	{
		return ERROR_NONE;
	} else {
		return ERROR_MQ131_READ_DATA_FAILED;
	}
}


/**
 * @brief Get O3 concentration from MQ131 Sensor
 * 
 * @param _sensorData_st pointer to struct data sensor
 * @return ERROR_CODE 
 */
ERROR_CODE MQ131_getData(struct sensorData *_sensorData_st)
{
	MQ131.setEnv(_sensorData_st->temperature, _sensorData_st->humidity);
	if (isTimeToReadData() == ERROR_NONE)
	{
		_sensorData_st->o3_ppb = MQ131.getO3(MQ131Unit::PPB);
		// lay gia tri min  cua O3 theo don vi ppb
		_sensorData_st->o3_ppb_min  = (_sensorData_st->o3_ppb_min < _sensorData_st->o3_ppb ) ? _sensorData_st->o3_ppb_min : _sensorData_st->o3_ppb;
		// lay gia tri max  cua O3 theo don vi ppb
		_sensorData_st->o3_ppb_max  = (_sensorData_st->o3_ppb_max > _sensorData_st->o3_ppb ) ? _sensorData_st->o3_ppb_max : _sensorData_st->o3_ppb;


		_sensorData_st->o3_ppm = MQ131.getO3(MQ131Unit::PPM);
		// lay gia tri min  cua O3 theo don vi ppm
		_sensorData_st->o3_ppm_min  = (_sensorData_st->o3_ppm_min < _sensorData_st->o3_ppm ) ? _sensorData_st->o3_ppm_min : _sensorData_st->o3_ppm;
		// lay gia tri max  cua O3 theo don vi ppm
		_sensorData_st->o3_ppm_max  = (_sensorData_st->o3_ppm_max > _sensorData_st->o3_ppm ) ? _sensorData_st->o3_ppm_max : _sensorData_st->o3_ppm;


		_sensorData_st->o3_ug  = MQ131.getO3(MQ131Unit::UG_M3);
		// lay gia tri min  cua O3 theo don vi ug
		_sensorData_st->o3_ug_min  = (_sensorData_st->o3_ug_min < _sensorData_st->o3_ug ) ? _sensorData_st->o3_ug_min : _sensorData_st->o3_ug;
		// lay gia tri max  cua O3 theo don vi ug
		_sensorData_st->o3_ug_max  = (_sensorData_st->o3_ug_max > _sensorData_st->o3_ug ) ? _sensorData_st->o3_ug_max : _sensorData_st->o3_ug;


		LOG_PRINT_INFORMATION("o3_ppb: %.1f", 		_sensorData_st->o3_ppb);
		LOG_PRINT_INFORMATION("o3_ppm: %.1f", 		_sensorData_st->o3_ppm);
		LOG_PRINT_INFORMATION("o3_ug: %.1f", 		_sensorData_st->o3_ug);
		LOG_PRINT_INFORMATION("o3_ppb_min: %.1f", 	_sensorData_st->o3_ppb_min);
		LOG_PRINT_INFORMATION("o3_ppm_min: %.1f", 	_sensorData_st->o3_ppm_min);
		LOG_PRINT_INFORMATION("o3_ug_min: %.1f", 	_sensorData_st->o3_ug_min);
		LOG_PRINT_INFORMATION("o3_ppb_max: %.1f",	_sensorData_st->o3_ppb_max);
		LOG_PRINT_INFORMATION("o3_ppm_max: %.1f", 	_sensorData_st->o3_ppm_max);
		LOG_PRINT_INFORMATION("o3_ug_max: %.1f", 	_sensorData_st->o3_ug_max);
		LOG_PRINT_INFORMATION("MQ131 Sensor read data successfully!");

		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("MQ131 Sensor read data failed!");
		return ERROR_MQ131_READ_DATA_FAILED;
	}
}

#endif
