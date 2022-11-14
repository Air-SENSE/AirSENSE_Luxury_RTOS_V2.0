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

ERROR_CODE MQTT_initClient( char *_topic, char *_espID, PubSubClient& _mqttClient, struct connectionStatus _connecctionStatus);
ERROR_CODE MQTT_postData(struct sensorData _sensorData_st, struct connectionStatus 	_connecctionStatus, PubSubClient& _mqttClient, NTPClient& _timeClient, const char *_nameDevice);
ERROR_CODE createMessageMQTTString( char *messageMQTTString, const char *_espID, NTPClient& _timeClient, struct sensorData _sensorData_st);

ERROR_CODE SDcard_init(uint8_t _pinSCK = PIN_NUM_CLK, uint8_t _pinMISO = PIN_NUM_MISO, uint8_t _pinMOSI = PIN_NUM_MOSI, uint8_t _pinCS = PIN_CS_SD_CARD, struct connectionStatus *_connecctionStatus = &connectionStatus_st);
ERROR_CODE SDcard_readFile( const char *fileName_string, char *fileContent_string, const uint32_t numberOfRequestedByte_u32, uint32_t *numberOfReadByte_u32);
ERROR_CODE SDcard_readCalibDataFromFile(struct connectionStatus *_connectStatus, const char *_nameFile, const char *_format, ...);
ERROR_CODE SDcard_saveStringDataToFile(struct connectionStatus *_connectStatus, const char *fileContent_string, struct sensorData  _sensorData_st, struct connectionStatus *_connecctionStatus);
ERROR_CODE createSensorDataString();

ERROR_CODE SHT_init(TwoWire &wire);
ERROR_CODE SHT_getData(const uint16_t temperature_calibInt_u16, const uint16_t humidity_calibInt_u16, float *temperature, float *humidity);

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
ERROR_CODE isSensorDisconnected(struct status_et _statusDevice);
ERROR_CODE isAllSensorDisconnected(struct connectionStatus _connectionStatus);


//==========================     SETUP       ========================

TaskHandle_t WIFI_smartConfig_Handle = NULL;
TaskHandle_t Sensors_getData_Handle = NULL;
TaskHandle_t Screen_display_Handle = NULL;
TaskHandle_t MQTT_sendData_Handle = NULL;
TaskHandle_t SD_writeData_Handle = NULL;

QueueHandle_t dataSensor_u32_Handle;
QueueHandle_t dataSensor_fl_Handle;
QueueHandle_t dataCalib_Handle;


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

void getCalibData_(void *parameters)
{
	calibData calibDataTemp_st;
	Screen_getCalibDataFromUser(calibDataTemp_st);
	
}

void getDataFromSensor_Task(void *parameters)
{
	for(;;)
	{
		if (connectionStatus_st.shtSensor == status_et::CONNECTED)
		{
			SHT_getData(calibData_st.temperature_calibInt_u32,
						calibData_st.humidity_calibInt_u32,
						sensorData_st.temperature,
						sensorData_st.humidity);
		} else {
			LOG_PRINT_INFORMATION("SHTsensor disconnected!");
		}

		if (connectionStatus_st.tflp01Sensor == status_et::CONNECTED)
		{
			TFLP01_getData(sensorData_st, calibData_st);

		}
		

		if (uxQueueSpacesAvailable(dataSensor_u32_Handle) > 0)
		{
			xQueueSendToBack(dataSensor_u32_Handle, &(sensorData_st.temperature), 0);
			xQueueSendToBack(dataSensor_u32_Handle, &(sensorData_st.humidity), 0);
		} else {
			LOG_PRINT_INFORMATION("Queue is full!");
		}
		
		vTaskDelay((TickType_t) 3000 / portTICK_PERIOD_MS);
	}	
}


/*#if(0)
void Wifi_checkStatus_Task(void *parameters)
{
	for(;;)
	{
		if (WiFi.status() == wl_status_t::WL_CONNECTED)			// kiem tra tinh trang ket noi WIFI
		{
			WiFi.
			TFT_wifiStatus = WIFI_Status_et::WIFI_CONNECTED;	// cap nhat trang thai WIFI
		}
		else
		{
			TFT_wifiStatus = WIFI_Status_et::WIFI_DISCONNECT;	// cap nhat trang thai WIFI
		}
		vTaskDelay(WIFI_TASK_DELAY);
	}
}

void Sensors_getData_Task(void *parameters)
{
	for(;;)
	{
		#ifdef O3_SENSOR
			O3_getData();
		#endif
		
		SHT_getData(temperature_calibInt_u16,
					humidity_calibInt_u16,
					&TFT_temperature_C,
					&TFT_humidity_percent);
		TFLP01_getData();
		DS3231_getData();
		vTaskDelay(SAMPLING_PERIOD);
	}
}

void Screen_display_Task(void *parameters) 
{
	for(;;)
	{
		Screen_displayData();
		vTaskDelay(TASK_DELAY);
	}
}

void MQTT_sendData_Task(void *parameters)
{
	for (;;)
	{
		MQTT_postData(TFT_humidity_percent, TFT_temperature_C, TFT_pm1_u32, TFT_pm25_u32, TFT_pm10_u32, TFT_o3_ppb_u32, TFT_o3_ppm, TFT_o3_ug, pm25_min_u32, pm25_max_u32);
		mqttClient.loop();

		vTaskDelay(MQTT_TASK_DELAY);
	}
}


void SD_writeData_Task(void *parameters)
{
	for(;;)
	{
		SDcard_saveDataToFile(TFT_humidity_percent, TFT_temperature_C, TFT_pm1_u32, TFT_pm25_u32, TFT_pm10_u32, TFT_o3_ppb_u32, TFT_o3_ppm, TFT_o3_ug, pm25_min_u32, pm25_max_u32);
		SD_runProgram();

		vTaskDelay(SD_TASK_DELAY);
	}
}



void setup()
{
	// core 0
	xTaskCreatePinnedToCore(SmartConfig_Task,
							"smart config",
							STACK_SIZE,
							NULL,
							1,
							&WIFI_SmartConfig_Handle,
							0 			// core 0
							);


	xTaskCreatePinnedToCore(Wifi_checkStatus_Task,
							"wifi status",
							STACK_SIZE,
							NULL,
							1,
							NULL,
							0 			// core 0
							);
	
#ifdef USING_MQTT

	xTaskCreatePinnedToCore(MQTT_sendData_Task,
							"SendDatatoMQTT",
							STACK_SIZE,
							NULL,
							1,
							&MQTT_sendData_Handle,
							0 			// core 0
							);

#endif

#ifdef USING_SD_CARD
	xTaskCreatePinnedToCore(SD_writeData_Task,
							"WtiteDatatoSD",
							STACK_SIZE,
							NULL,
							1,
							&SD_writeData_Handle,
							0			// core 0 
							);

#endif


	// core 1
	xTaskCreatePinnedToCore(Sensors_getData_Task,
							"SensorGetData",
							STACK_SIZE,
							NULL,
							1,
							&Sensors_getData_Handle,
							1 			// core 1
							);


	xTaskCreatePinnedToCore(Screen_display_Task,
							"Screen dislay",
							STACK_SIZE,
							NULL,
							1,
							&Screen_display_Handle,
							1 			// core 1
							);
	
}

#endif*/


void setup()
{
    DEBUG_SERIAL_PORT.begin(9600);

	WIFI_init();
	initAllSensor();
	
#ifdef USING_MQTT
	MQTT_initClient(topic, espID, mqttClient, connectionStatus_st);
	timeClient.begin();
#endif

#ifdef USING_SDCARD
	LOG_PRINT_INFORMATION("Check SD");
	SDcard_init(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD, connectionStatus_st);
#endif

	dataSensor_u32_Handle = xQueueCreate(11, sizeof(uint32_t));
	dataSensor_fl_Handle  = xQueueCreate(5, sizeof(float));
	dataCalib_Handle	  = xQueueCreate(7, sizeof(uint32_t));

	wifiTimer = xTimerCreate("ButtonTimer", BUTTON_PRESS_DURATION, pdFALSE, (void *) 0, smartConfigWiFi);

	LOG_PRINT_INFORMATION("Create task!");
	xTaskCreate(smartConfigWiFi_Task, "Smart config", STACK_SIZE, NULL, 3, &WIFI_smartConfig_Handle);

}

//==========================     LOOP       ========================

void loop()
{

}
