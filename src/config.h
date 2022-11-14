/**
 * @file config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
//========================== DEFINE CHO TOAN BO CODE ========================

#define DEBUG_SERIAL    
#define DEBUG_SERIAL_PORT       Serial
#define USING_SDCARD
#define USING_MQTT
#define SERIAL_DEBUG_BAUDRATE   115200

//========================== DEFINE O3 SENSOR ========================

// #define SSID
// #define PASSWORD

#define TFLP01_SERIAL_PORT_BAUDRATE 115200
#define TFLP01_SERIAL_PORT Serial2

#define O3_SENSOR_DF_ROBOT
#define O3_SENSOR_MQ131

#define PIN_MQ131_POWER      2
#define PIN_MQ131_SENSOR     4  

//========================== DEFINE CHO MAN HINH ========================

#define SCREEN_SERIAL_PORT      Serial
#define ERROR_READ_DISPLAY      777777
#define NEXTION_BAUDRATE        9600
#define EEPROM_SIZE             512
#define PIN_TX_TFT              33
#define PIN_RX_TFT              25

//========================== DEFINE CHO SENSOR TF-LP01 ========================

#define PIN_TX_TFLP01   17
#define PIN_RX_TFLP01   16

//========================== DEFINE CHO SD CARD  ========================

#define PIN_NUM_MISO    21
#define PIN_NUM_MOSI    19
#define PIN_NUM_CLK     18
#define PIN_CS_SD_CARD  5

#define ERROR_CODE int
#define ERROR_NONE 0x00

//========================== DEFINE CHO MAN HINH ========================

#define USING_RTC
#define PIN_SDA_GPIO    26
#define PIN_SCL_GPIO    27
#define I2C_CLOCK_SPEED 100000U
#define RTC_I2C_ADDRESS 0x68

//========================== DEFINE CHO MAN HINH ========================

#define PIN_NEO_PIXEL   23

//========================== DEFINE CHO MAN HINH ========================

#define PIN_BUTTON_1    35
#define PIN_BUTTON_2    34

//========================== DEFINE THIET BI ========================

#define NAME_DEVICE "AirSENSE_RTOS"
#define NUM_TIMERS   1

// dinh nghia kieru gia tri cho cam bien
#define FIXPOINT_32_16 uint32_t    // kieru du lieu fix point dung cho cam bien

//========================== khai bao cac bien hien thi tren man hinh ========================
//=========================== Define cac chu ki =============================

#define MQTT_SENDDATA_PERIOD	    (uint32_t)10000
#define SD_WRITEDATA_PERIOD		    (uint32_t)10000
#define WIFI_RECONNECT_PERIOD	    (uint32_t)6000
#define BUTTON_PRESSED_DURATION 	(uint32_t)4000
#define WIFI_MAX_CONNECT_TRIAL      (uint8_t)10


#define BUTTON_PRESS_DURATION	((TickType_t) 3000 / portTICK_PERIOD_MS)
#define TASK_DELAY       ((TickType_t) 1000 / portTICK_PERIOD_MS)
#define WIFI_TASK_DELAY  ((TickType_t) WIFI_RECONNECT_PERIOD / portTICK_PERIOD_MS)
#define MQTT_TASK_DELAY  ((TickType_t) MQTT_SENDDATA_PERIOD / portTICK_PERIOD_MS)
#define SD_TASK_DELAY    ((TickType_t) SD_WRITEDATA_PERIOD / portTICK_PERIOD_MS)
#define STACK_SIZE       ((1024U) * 10)     // byte


char dateTime_string[25];
String messageData;

