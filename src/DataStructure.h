/**
 * @file DataStructure.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include <string>

#define ERROR_DATA_CREATE_STRINGDATA_FAILED 0x91

struct calibData {
    uint32_t temperature_calibInt_u32;
    uint32_t humidity_calibInt_u32;
    uint32_t pm1_calibInt_u32;
    uint32_t pm10_calibInt_u32;
    uint32_t pm25_calibInt_u32;
    uint32_t temperature_calibFloat_u32;
    uint32_t humidity_calibFloat_u32;

    calibData() {     // initialation function for struct (C++ style)
        this->temperature_calibInt_u32      = 0;
        this->humidity_calibInt_u32         = 0;
        this->pm1_calibInt_u32              = 0;
        this->pm10_calibInt_u32             = 0;
        this->pm25_calibInt_u32             = 0;
        this->temperature_calibFloat_u32    = 0;
        this->humidity_calibFloat_u32       = 0;
    }
};

calibData calibData_st;


/**
 * @brief Create a string from sensor data for storing in the SD card
 * 
 * @param[out] _calibDataString: pointer char*, use to store string
 * @param[in]  _calibData: struct calibdata
 * @return ERROR_CODE 
 */
ERROR_CODE creatCalibDataString(char *_calibDataString, struct calibData _calibData)
{
	sprintf(_calibDataString, "%u|%u|%u|%u|%u|%u|%u\n",
							  _calibData.temperature_calibInt_u32,
							  _calibData.humidity_calibInt_u32,
							  _calibData.pm1_calibInt_u32,
							  _calibData.pm10_calibInt_u32,
							  _calibData.pm25_calibInt_u32,
							  _calibData.temperature_calibFloat_u32,
							  _calibData.humidity_calibFloat_u32 );

	if (_calibDataString[0] == '\0')
	{
		LOG_PRINT_ERROR("Create string failed!");
		return ERROR_DATA_CREATE_STRINGDATA_FAILED;
	} else {
		LOG_PRINT_ERROR("Create string complete!");
		return ERROR_NONE;
	}
}


struct sensorData {
    float      temperature;
    float      humidity;
    uint32_t   pm1_u32;
    uint32_t   pm25_u32;
    uint32_t   pm10_u32;
    uint32_t   pm25_min_u32;
    uint32_t   pm25_max_u32;
    float      o3_ppb;
    float      o3_ppm;
    float      o3_ug;
    float      o3_ppb_min;
    float      o3_ppm_min;
    float      o3_ug_min;
    float      o3_ppb_max;
    float      o3_ppm_max;
    float      o3_ug_max;

    sensorData() 
    {
        this->temperature	  = 20.5;
        this->humidity		  = 60;
        this->pm1_u32         = 0;
        this->pm25_u32        = 0;
        this->pm10_u32        = 0;
        this->pm25_min_u32    = 0;
        this->pm25_max_u32    = 0;
        this->o3_ppb          = 0;
        this->o3_ppm          = 0;
        this->o3_ug           = 0;
        this->o3_ppb_min      = 100;
        this->o3_ppm_min      = 100;
        this->o3_ug_min       = 100;
        this->o3_ppb_max      = 0;
        this->o3_ppm_max      = 0;
        this->o3_ug_max       = 0;
    };

    sensorData(QueueHandle_t queue1, QueueHandle_t queue2, QueueHandle_t queue3)
    {
        if (uxQueueSpacesAvailable(queue1))
        {
            xQueueReceive(queue1, (void *)&(this->humidity   ), NO_WAIT);
            xQueueReceive(queue1, (void *)&(this->temperature), NO_WAIT);
        }
        
        if (uxQueueSpacesAvailable(queue2))
        {
            xQueueReceive(queue2, (void *)&(this->pm1_u32     ), NO_WAIT);
            xQueueReceive(queue2, (void *)&(this->pm25_u32    ), NO_WAIT);
            xQueueReceive(queue2, (void *)&(this->pm10_u32    ), NO_WAIT);
            xQueueReceive(queue2, (void *)&(this->pm25_min_u32), NO_WAIT);
            xQueueReceive(queue2, (void *)&(this->pm25_max_u32), NO_WAIT);
        }

        if (uxQueueSpacesAvailable(queue3))
        {
            xQueueReceive(queue3, (void *)&(this->o3_ppb     ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ppm     ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ug      ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ppb_min ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ppm_min ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ug_min  ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ppb_max ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ppm_max ), NO_WAIT);
            xQueueReceive(queue3, (void *)&(this->o3_ug_max  ), NO_WAIT);
        }
        
    }

    sensorData operator=(const sensorData _sensorData_temp)
    {
        this->temperature	 = _sensorData_temp.temperature;
        this->humidity		 = _sensorData_temp.humidity;
        this->pm1_u32        = _sensorData_temp.pm1_u32;
        this->pm25_u32       = _sensorData_temp.pm25_u32;
        this->pm10_u32       = _sensorData_temp.pm10_u32;
        this->pm25_min_u32   = _sensorData_temp.pm25_min_u32;
        this->pm25_max_u32   = _sensorData_temp.pm25_max_u32;
        this->o3_ppb         = _sensorData_temp.o3_ppb;
        this->o3_ppm         = _sensorData_temp.o3_ppm;
        this->o3_ug          = _sensorData_temp.o3_ug;
        this->o3_ppb_min     = _sensorData_temp.o3_ppb_min;
        this->o3_ppm_min     = _sensorData_temp.o3_ppm_min;
        this->o3_ug_min      = _sensorData_temp.o3_ug_min;
        this->o3_ppb_max     = _sensorData_temp.o3_ppb_max;
        this->o3_ppm_max     = _sensorData_temp.o3_ppm_max;
        this->o3_ug_max      = _sensorData_temp.o3_ug_max;

        return _sensorData_temp;
    }
};

struct sensorData sensorData_st;

/**
 * @brief Create a string
 * 
 * @param[out] _calibDataString: pointer char*, use to store string
 * @param[in]  _calibData: struct calibdata
 * @return ERROR_CODE 
 */
ERROR_CODE createSensorDataString(char *_sensorDataString,
                                  const char *_nameDevice,
                                  const char *_dateTimeString,
                                  struct sensorData _sensorData_st)
{
    sprintf(_sensorDataString, "%s,%s,%.1f,%.1f,%u,%u,%u,%u,%u,%u,%.3f,%.1f\n",
                                _nameDevice,
                                _dateTimeString,
                                _sensorData_st.temperature,
                                _sensorData_st.humidity,
                                _sensorData_st.pm1_u32,
                                _sensorData_st.pm25_u32,
                                _sensorData_st.pm10_u32,
                                _sensorData_st.pm25_min_u32,
                                _sensorData_st.pm25_max_u32,
                                _sensorData_st.o3_ppb,
                                _sensorData_st.o3_ppm,
                                _sensorData_st.o3_ug  );

	if (_sensorDataString[0] == '\0')
	{
		LOG_PRINT_ERROR("Create string failed!");
		return ERROR_DATA_CREATE_STRINGDATA_FAILED;
	} else {
		LOG_PRINT_ERROR("Create string complete!");
		return ERROR_NONE;
	}
}


ERROR_CODE createMessageMQTTString( String &messageMQTTString,
                                    const char *_espID,
                                    NTPClient& _timeClient,
                                    struct sensorData _sensorData_st)
{
	_timeClient.update();

    DynamicJsonDocument doc(256);
    JsonObject data = doc.createNestedObject("DATA");

    doc["StationID"] = _espID;
    doc["Time"] = _timeClient.getEpochTime();
    
    data["Temperature"] = _sensorData_st.temperature;
    data["Humidity"] = _sensorData_st.humidity;
    data["PM1"]  = _sensorData_st.pm1_u32;
    data["PM25"] = _sensorData_st.pm25_u32;
    data["PM10"] = _sensorData_st.pm10_u32;
    data["PM25MAX"] = _sensorData_st.pm25_max_u32;
    data["PM25MIN"] = _sensorData_st.pm25_min_u32;
    data["O3_PPB"] = _sensorData_st.o3_ppb;
    data["O3_PPM"] = _sensorData_st.o3_ppm;
    data["O3_UG"]  = _sensorData_st.o3_ug;

    serializeJson(doc, messageMQTTString);

    return ERROR_NONE;
}


