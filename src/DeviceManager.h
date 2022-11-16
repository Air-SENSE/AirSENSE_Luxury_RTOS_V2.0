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



struct connectionStatus connectionStatus_st;


ERROR_CODE initAllSensor()
{
    if ((SHT_init(Wire) == ERROR_NONE)                                               &&
        (TFLP01_init(TFLP01_SERIAL_PORT, TFLP01_SERIAL_PORT_BAUDRATE, &connectionStatus_st) == ERROR_NONE) ||
        (DFRobotO3_init() == ERROR_NONE)                                             &&
        (MQ131Sensor_init() == ERROR_NONE)                                           &&
        (Button_init(PIN_BUTTON_1, INPUT, 1) == ERROR_NONE))
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
ERROR_CODE isSensorDisconnected(status_et _statusDevice)
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
    if (isSensorDisconnected(_connectionStatus.shtSensor)       == ERROR_SENSOR_DISCONNECTED  &&
        isSensorDisconnected(_connectionStatus.mq131Sensor)     == ERROR_SENSOR_DISCONNECTED  &&
        isSensorDisconnected(_connectionStatus.dfrobotO3Sensor) == ERROR_SENSOR_DISCONNECTED  &&
        isSensorDisconnected(_connectionStatus.tflp01Sensor)    == ERROR_SENSOR_DISCONNECTED )
    {
        LOG_PRINT_INFORMATION("All sensors disconnected");
        return ERROR_SENSOR_ALL_DISCONNECTED;
    } else {
        return ERROR_NONE;
    }
}

