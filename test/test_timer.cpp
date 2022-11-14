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


#define LOG_PRINT_ERROR(format, ...)
#define LOG_PRINT_NOTIFICATION(message)
#define LOG_PRINT_INFORMATION(format, ...)
#define LOG_PRINT_ASSERT_INFOR(condition, message)
#define LOG_PRINT_ASSERT_INFOR2(condition, message1, message2)
#define LOG_PRINT_ASSERT_ERROR(condition, message)


#include "config.h"
#include "DataStructure.h"
#include "MQTTConnection.h"
#include "log.h"
#include "SHT85Sensor.h"
#include "TFLP01Driver.h"
#include "file.h"
#include "UI.h"
#include "MQ131Driver.h"
#include "DFRobotO3.h"
#include "ButtonDriver.h"

//========================== Cac ham su dung ========================


ERROR_CODE DS3231_init(RTC_DS3231& _realTime, NTPClient&  _timeClient, TwoWire &_wire, struct connectionStatus _connectionStatus);
ERROR_CODE DS3231_getStringDateTime(RTC_DS3231 _realTime, char *_format, char *_dateTime_string);

ERROR_CODE MQTT_initClient( char *_topic, char *_espID, PubSubClient& _mqttClient, struct connectionStatus _connecctionStatus);
ERROR_CODE MQTT_postData(struct sensorData _sensorData_st, struct connectionStatus 	_connecctionStatus, PubSubClient& _mqttClient, NTPClient& _timeClient, const char *_nameDevice);

ERROR_CODE SDcard_init(uint8_t _pinSCK = PIN_NUM_CLK, uint8_t _pinMISO = PIN_NUM_MISO, uint8_t _pinMOSI = PIN_NUM_MOSI, uint8_t _pinCS = PIN_CS_SD_CARD, struct connectionStatus *_connecctionStatus = &connectionStatus_st);
ERROR_CODE SDcard_readFile( const char *fileName_string, char *fileContent_string, const uint32_t numberOfRequestedByte_u32, uint32_t *numberOfReadByte_u32);
ERROR_CODE SDcard_readCalibDataFromFile(const char *_nameFile, const char *_format, ...);
ERROR_CODE SDcard_saveStringDataToFile( const char *fileContent_string, struct sensorData  _sensorData_st, struct connectionStatus *_connecctionStatus);

ERROR_CODE SHT_init(TwoWire &wire);
ERROR_CODE SHT_getData(const uint16_t temperature_calibInt_u16, const uint16_t humidity_calibInt_u16, float *temperature, float *humidity);

ERROR_CODE TFLP01_init(uint32_t baudRate_u32 = 115200);
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

ERROR_CODE DFRobotO3_init();
ERROR_CODE DFRobotO3_getData(struct sensorData *_sensorData_st);

void MQ131Sensor_init();
ERROR_CODE isTimeToReadData();
ERROR_CODE MQ131_getData(struct sensorData *_sensorData_st);

bool Button_isLongPressed();


//==========================     SETUP       ========================


TaskHandle_t WIFI_SmartConfig_Handle = NULL;
TaskHandle_t Sensors_getData_Handle = NULL;
TaskHandle_t Screen_display_Handle = NULL;
TaskHandle_t MQTT_sendData_Handle = NULL;
TaskHandle_t SD_writeData_Handle = NULL;

TimerHandle_t wifiTimer;


void smartConfigWiFi(TimerHandle_t xTimer)
{
	LOG_PRINT_INFORMATION("--> Wifi is scanning....");
	for (size_t wifi_connectTrialCount_u8 = 0; wifi_connectTrialCount_u8 < WIFI_MAX_CONNECT_TRIAL; wifi_connectTrialCount_u8++)
	{
		if (WiFi.smartConfigDone())
		{
			connectionStatus_st.wifiStatus = status_et::CONNECTED;
			Screen_updateStatus(&connectionStatus_st);
			LOG_PRINT_INFORMATION("Wifi connected!");
			LOG_PRINT_INFORMATION("Host name: %s.", WiFi.getHostname);
			break;
		}
	}

	connectionStatus_st.wifiStatus = status_et::DISCONNECTED;
	Screen_updateStatus(&connectionStatus_st);
	LOG_PRINT_INFORMATION("Wifi disconnected!");
}

void smartConfig_Task(void *parameters)
{
	for (;;)
	{
		if (digitalRead(PIN_BUTTON_1) == 1)
		{
			connectionStatus_st.wifiStatus = status_et::SCANNING;
			Screen_updateStatus(&connectionStatus_st);
			xTimerStart(wifiTimer, 0);
		}
	}
}

void setup(){
    DEBUG_SERIAL_PORT.begin(SERIAL_DEBUG_BAUDRATE);

    WiFi.begin();

	wifiTimer = xTimerCreate("ButtonTimer", BUTTON_PRESS_DURATION, pdFALSE, (void *) 0, smartConfigWiFi);

}


void loop() {

}