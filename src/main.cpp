/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

//========================== Khai bao thu vien ========================

#include <Arduino.h>
#include <stdarg.h>
#include <string>
#include "Wire.h"
#include <SPI.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <ArduinoJson.h>


#define LOG_PRINT_ERROR(format, ...)
#define LOG_PRINT_NOTIFICATION(message)
#define LOG_PRINT_INFORMATION(format, ...)
#define LOG_PRINT_ASSERT_INFOR(condition, message)
#define LOG_PRINT_ASSERT_INFOR2(condition, correctMessage, incorrectMessage)
#define LOG_PRINT_ASSERT_ERROR(condition, message)


#include "config.h"
#include "DeviceManager.h"
#include "DataStructure.h"
#include "log.h"
#include "WiFiManager.h"
#include "MQTTConnection.h"
#include "SHT85Sensor.h"
#include "TFLP01Driver.h"
#include "file.h"
#include "UI.h"
#include "MQ131Driver.h"
#include "DFRobotO3.h"
#include "ButtonDriver.h"

//==========================  INCLUDE FUNCTION  ========================

ERROR_CODE WIFI_init();

ERROR_CODE DS3231_init(RTC_DS3231& _realTime, NTPClient&  _timeClient, TwoWire &_wire, struct connectionStatus _connectionStatus);
ERROR_CODE DS3231_getStringDateTime(RTC_DS3231 _realTime, char *_format, char *_dateTime_string);

ERROR_CODE MQTT_initClient( char *_topic, char *_espID, PubSubClient& _mqttClient, struct connectionStatus *_connecctionStatus);
ERROR_CODE MQTT_postData(struct sensorData _sensorData_st, struct connectionStatus 	_connecctionStatus, PubSubClient& _mqttClient, NTPClient& _timeClient, const char *_nameDevice);
ERROR_CODE createMessageMQTTString(String &messageMQTTString, const char *_espID, NTPClient& _timeClient, struct sensorData _sensorData_st);

ERROR_CODE SDcard_init(uint8_t _pinSCK = PIN_NUM_CLK, uint8_t _pinMISO = PIN_NUM_MISO, uint8_t _pinMOSI = PIN_NUM_MOSI, uint8_t _pinCS = PIN_CS_SD_CARD, struct connectionStatus *_connecctionStatus = &connectionStatus_st);
ERROR_CODE SDcard_readFile( const char *fileName_string, char *fileContent_string, const uint32_t numberOfRequestedByte_u32, uint32_t *numberOfReadByte_u32);
ERROR_CODE SDcard_readCalibDataFromFile(struct connectionStatus *_connectStatus, const char *_nameFile, const char *_format, ...);
ERROR_CODE SDcard_saveStringDataToFile(struct connectionStatus *_connectStatus, const char *fileContent_string);
ERROR_CODE createSensorDataString(char *_sensorDataString, const char *_nameDevice, const char *_dateTimeString, struct sensorData _sensorData_st);

ERROR_CODE SHT_init(TwoWire &wire, struct connectionStatus *_connectStatus);
ERROR_CODE SHT_getData(struct connectionStatus *_connectStatus, const uint16_t temperature_calibInt_u16, const uint16_t humidity_calibInt_u16, struct sensorData *_sensorData);

ERROR_CODE TFLP01_init(HardwareSerial& _stream, uint32_t baudRate_u32);
ERROR_CODE TFLP01_readRawData(uint8_t *dataArray, size_t lenght);
ERROR_CODE TFLP01_getData(struct sensorData *_sensorData_st, struct calibData *_calibData);

ERROR_CODE Screen_init(HardwareSerial& _stream, const uint32_t _baudRate_u32, const uint32_t _romSize_u32);
ERROR_CODE Screen_getDataFromTextBox(const char *objectName, uint32_t *calibValue);
ERROR_CODE Screen_getCalibDataFromUser(struct calibData *_calibData);
ERROR_CODE checkDataValid(struct calibData *_calibData);
ERROR_CODE Screen_displayCalibData(struct calibData *_calibData);
ERROR_CODE Screen_updateStatus(struct connectionStatus *_connectionStatus_st);
ERROR_CODE Screen_updateCurrentDateTime(const char *currentTimeString);
ERROR_CODE Screen_displaysensorData(struct sensorData *_sensorData_st);
ERROR_CODE creatCalibDataString(char *_calibDataString, struct calibData _calibData);

ERROR_CODE DFRobotO3_init();
ERROR_CODE DFRobotO3_getData(struct sensorData *_sensorData_st);

ERROR_CODE MQ131Sensor_init();
ERROR_CODE isTimeToReadData();
ERROR_CODE MQ131_getData(struct sensorData *_sensorData_st);

ERROR_CODE Button_init(uint8_t buttonPin, uint8_t mode, int defaultStatus = 1);
bool Button_isPressed(uint8_t buttonPin);

ERROR_CODE initAllSensor();
ERROR_CODE isSensorDisconnected(status_et _statusDevice);
ERROR_CODE isAllSensorDisconnected(struct connectionStatus _connectionStatus);


//==========================     SETUP       ========================

TaskHandle_t WIFI_smartConfig_Handle = NULL;
TaskHandle_t Sensors_getData_Handle = NULL;
TaskHandle_t Screen_display_Handle = NULL;
TaskHandle_t MQTT_sendData_Handle = NULL;
TaskHandle_t SD_writeData_Handle = NULL;

// QueueHandle_t dataSensor_u32_Handle;
// QueueHandle_t dataSensor_fl_Handle;

QueueHandle_t dataSHTSensorQueue;
QueueHandle_t dataTFLPSensorQueue;
QueueHandle_t dataO3Queue;
QueueHandle_t dataCalibrateQueue;


TimerHandle_t wifiTimer;

void smartConfigWiFi(TimerHandle_t xTimer)
{
	LOG_PRINT_INFORMATION("--> Wifi is scanning....");
	for (size_t wifi_connectTrialCount = 0; wifi_connectTrialCount < WIFI_MAX_CONNECT_TRIAL; wifi_connectTrialCount++)
	{
		if (WiFi.smartConfigDone())
		{
			connectionStatus_st.wifiStatus = status_et::CONNECTED;
			Screen_updateStatus(&connectionStatus_st);
			LOG_PRINT_INFORMATION("Wifi connected!");
			LOG_PRINT_INFORMATION("Host name: %s.", WiFi.getHostname);
		}
	}

	connectionStatus_st.wifiStatus = status_et::DISCONNECTED;
	Screen_updateStatus(&connectionStatus_st);
	LOG_PRINT_INFORMATION("Wifi disconnected!");
}


void smartConfigWiFi_Task(void *parameters)
{
	for (;;)
	{
		if ((xTimerIsTimerActive(wifiTimer) == pdFALSE) && Button_isPressed(PIN_BUTTON_1)) 
		{
			xTimerStart(wifiTimer, 0);
			connectionStatus_st.wifiStatus = status_et::SCANNING;
			Screen_updateStatus(&connectionStatus_st);
		} else if ((xTimerIsTimerActive(wifiTimer) == pdTRUE) && !Button_isPressed(PIN_BUTTON_1)) 
		{
			xTimerStop(wifiTimer, 0);
			connectionStatus_st.wifiStatus = status_et::DISCONNECTED;
			Screen_updateStatus(&connectionStatus_st);
		} else {
			continue;
		}
		vTaskDelay((TickType_t) 100 / portTICK_PERIOD_MS);
	}
}


void updateCalibrateDataFromScreen_Task(void *parameters)
{
	calibData calibDataTemp_st;
	for (;;)
	{
		Screen_getCalibDataFromUser(&calibDataTemp_st);
		DS3231_getStringDateTime(realTime, "hh:mm:ss/DD/MMM/YYYY", (char *)dateTime_string);
		creatCalibDataString(dataCalib_string, calibDataTemp_st);
		SDcard_saveStringDataToFile(&connectionStatus_st, (const char*)dataCalib_string);
		vTaskDelay((TickType_t) (60*60*1000) / portTICK_PERIOD_MS);
	}
}


void getDataFromSensor_Task(void *parameters)
{
	struct sensorData sensorDataTemp_st;
	struct calibData calibDataTemp_st;
	for(;;)
	{
		SDcard_readCalibDataFromFile(&connectionStatus_st, fileNameCalib, "%u|%u|%u|%u|%u|",
																		  &(calibDataTemp_st.temperature_calibInt_u32),
																		  &(calibDataTemp_st.humidity_calibInt_u32),
																		  &(calibDataTemp_st.pm1_calibInt_u32),
																		  &(calibDataTemp_st.pm25_calibInt_u32),
																		  &(calibDataTemp_st.pm10_calibInt_u32));

		if (connectionStatus_st.shtSensor == status_et::CONNECTED)
		{
			SHT_getData(&connectionStatus_st, calibDataTemp_st.temperature_calibInt_u32,
						calibDataTemp_st.humidity_calibInt_u32,
						&sensorData_st);

			if (uxQueueSpacesAvailable(dataSHTSensorQueue) < (UBaseType_t)2)
			{
				xQueueSend(dataSHTSensorQueue, &(sensorDataTemp_st.temperature), NO_WAIT);
				xQueueSend(dataSHTSensorQueue, &(sensorDataTemp_st.humidity), NO_WAIT);
			}

		} else {
			LOG_PRINT_INFORMATION("SHT sensor disconnected!");
		}

		if (connectionStatus_st.tflp01Sensor == status_et::CONNECTED)
		{
			TFLP01_getData(&sensorDataTemp_st, &calibDataTemp_st);
			
			if (uxQueueSpacesAvailable(dataTFLPSensorQueue) < (UBaseType_t)5)
			{
				xQueueSend(dataTFLPSensorQueue, &(sensorDataTemp_st.pm1_u32), NO_WAIT);
				xQueueSend(dataTFLPSensorQueue, &(sensorDataTemp_st.pm25_u32), NO_WAIT);
				xQueueSend(dataTFLPSensorQueue, &(sensorDataTemp_st.pm10_u32), NO_WAIT);
				xQueueSend(dataTFLPSensorQueue, &(sensorDataTemp_st.pm1_u32), NO_WAIT);
				xQueueSend(dataTFLPSensorQueue, &(sensorDataTemp_st.pm1_u32), NO_WAIT);
			}

		} else {
			LOG_PRINT_INFORMATION("TFLP01 sensor disconnected!");
		}

#if defined(O3_SENSOR_MQ131)
		if (connectionStatus_st.mq131Sensor == status_et::CONNECTED)
		{
			MQ131_getData(&sensorDataTemp_st);

			if (uxQueueSpacesAvailable(dataO3Queue) < (UBaseType_t)9)
			{
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb_max), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm_max), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug_max), NO_WAIT);
			}

		} else {
			LOG_PRINT_INFORMATION("MQ131 sensor disconnected!");
		}

#elif defined(O3_SENSOR_DF_ROBOT)
		if (connectionStatus_st.dfrobotO3Sensor == status_et::CONNECTED)
		{
			DFRobotO3_getData(&sensorDataTemp_st);

			if (uxQueueSpacesAvailable(dataO3Queue) < (UBaseType_t)9)
			{
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug_min), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppb_max), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ppm_max), NO_WAIT);
				xQueueSend(dataO3Queue, &(sensorDataTemp_st.o3_ug_max), NO_WAIT);
			}

		} else {
			LOG_PRINT_INFORMATION("DFROBOT sensor disconnected!");
		}
#endif
		sensorData_st = sensorDataTemp_st;
		vTaskDelay(SAMPLING_PERIOD);
	}	
}

void dataManagement_Task(void *parameters)
{
	for (;;)
	{
		struct sensorData sensorDataTemp_st(dataSHTSensorQueue, dataTFLPSensorQueue, dataO3Queue);

		DS3231_getStringDateTime(realTime, "hh:mm:ss,DD/MMM/YYYY ", dateTime_string);
		createSensorDataString(sensorDataString, NAME_DEVICE, dateTime_string,sensorDataTemp_st);
		DS3231_getStringDateTime(realTime, "DD/MMM/YYYY", nameFileSaveData);
		SDcard_saveStringDataToFile(&connectionStatus_st, sensorDataString);

		createMessageMQTTString(messageData, (const char *)espID, timeClient, sensorDataTemp_st);
		// MQTT_postData(messageData.c_str(), &connectionStatus_st, mqttClient);

		vTaskDelay(((TickType_t) 5000 / portTICK_PERIOD_MS));
	}
	
}


void setup()
{
    DEBUG_SERIAL_PORT.begin(SERIAL_DEBUG_BAUDRATE);

	WIFI_init();
	//initAllSensor();

	SHT_init(Wire, &connectionStatus_st);
	TFLP01_init(TFLP01_SERIAL_PORT, TFLP01_SERIAL_PORT_BAUDRATE, &connectionStatus_st);
	DFRobotO3_init();
	MQ131Sensor_init();
	Button_init(PIN_BUTTON_1, INPUT, 1);
	
#ifdef USING_MQTT
	MQTT_initClient(topic, espID, mqttClient, &connectionStatus_st);
	timeClient.begin();
#endif

#ifdef USING_SDCARD
	LOG_PRINT_INFORMATION("Check SD");
	SDcard_init(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD, &connectionStatus_st);
#endif

	// dataSensor_u32_Handle = xQueueCreate(11, sizeof(uint32_t));
	// dataSensor_fl_Handle  = xQueueCreate(5, sizeof(float));
	dataCalibrateQueue	= xQueueCreate(7, sizeof(uint32_t));

	dataSHTSensorQueue	= xQueueCreate(2, sizeof(float));
	dataTFLPSensorQueue	= xQueueCreate(5, sizeof(uint32_t));
	dataO3Queue			= xQueueCreate(9, sizeof(float));

	wifiTimer = xTimerCreate("ButtonTimer", BUTTON_PRESS_DURATION, pdFALSE, (void *) 0, smartConfigWiFi);

	LOG_PRINT_INFORMATION("Create task!");
	xTaskCreatePinnedToCore(smartConfigWiFi_Task, "Smart config", STACK_SIZE, NULL, 4, &WIFI_smartConfig_Handle, 0);
	xTaskCreatePinnedToCore(dataManagement_Task, "Data management", STACK_SIZE, NULL, 4, NULL, 0);
	xTaskCreatePinnedToCore(getDataFromSensor_Task, "Get sensor data", STACK_SIZE, NULL, 3, NULL, 1);
};

//==========================     LOOP       ========================

void loop()
{
	// nothing
}
