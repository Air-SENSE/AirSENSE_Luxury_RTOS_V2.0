/**
 * @file DeviceManager.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once 

#include "config.h"
#include "log.h"
#include "MQTTConnection.h"
#include "file.h"
#include "SHT85Sensor.h"
#include "TFLP01Driver.h"
#include "MQ131Driver.h"
#include "DFRobotO3.h"
#include "ButtonDriver.h"


#define DEVICE_CONNECTED                ERROR_NONE
#define ERROR_SENSOR_DISCONNECTED       0xc1
#define ERROR_SENSOR_ALL_DISCONNECTED   0xc2
#define ERROR_SENSOR_INIT_FAILED        0xc3

typedef enum 
{
    DISCONNECTED,
    CONNECTED,
    CONNECTION_LOST,
    SCANNING,
    SENDING_DATA,
    READING_DATA,
    WRITING_DATA
} status_et;


struct connectionStatus
{
    status_et wifiStatus;
    status_et sdCardStatus;
    status_et mqttConnection;
    status_et ds3231Module;
    status_et tflp01Sensor;
    status_et dfrobotO3Sensor;
    status_et shtSensor;
    status_et mq131Sensor;
} connectionStatus_st;

ERROR_CODE initAllSensor()
{
    if ((SHT_init(Wire) == ERROR_NONE)                                               &&
        (TFLP01_init(TFLP01_SERIAL_PORT, TFLP01_SERIAL_PORT_BAUDRATE) == ERROR_NONE) ||
        (DFRobotO3_init() == ERROR_NONE)                                             &&
        (MQ131Sensor_init() == ERROR_NONE)                                           &&
        (Button_init(PIN_BUTTON_1, INPUT) == ERROR_NONE))
    {
        return ERROR_NONE;
    } else {
        return ERROR_SENSOR_INIT_FAILED;
    }
}

/**
 * @brief 
 * 
 * @param _statusDevice 
 * @return ERROR_CODE 
 */
ERROR_CODE isSensorDisconnected(struct status_et _statusDevice)
{
    if (_statusDevice == status_et::DISCONNECTED)
    {
        return ERROR_SENSOR_DISCONNECTED;
    } else {
        return DEVICE_CONNECTED;
    }
}


/**
 * @brief 
 * 
 * @param _connectionStatus 
 * @return ERROR_CODE 
 */
ERROR_CODE isAllSensorDisconnected(struct connectionStatus _connectionStatus)
{
    if (isDeviceDisconnected(_connectionStatus.shtSensor)       == ERROR_SENSOR_DISCONNECTED  &&
        isDeviceDisconnected(_connectionStatus.mq131Sensor)     == ERROR_SENSOR_DISCONNECTED  &&
        isDeviceDisconnected(_connectionStatus.dfrobotO3Sensor) == ERROR_SENSOR_DISCONNECTED  &&
        isDeviceDisconnected(_connectionStatus.tflp01Sensor)    == ERROR_SENSOR_DISCONNECTED )
    {
        LOG_PRINT_INFORMATION("All sensors disconnected");
        return ERROR_SENSOR_ALL_DISCONNECTED;
    } else {
        return ERROR_NONE;
    }
}

