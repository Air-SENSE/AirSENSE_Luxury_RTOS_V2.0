//========================== Khai bao thu vien ========================

#include <Arduino.h>
#include <string>
#include "Wire.h"
#include <SPI.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <FreeRTOS.h>



//========================== Cac ham su dung ========================


void DS3231_init();
void DS3231_getData();

void MQTT_initClient(char* _topic, char* _espID, PubSubClient& _mqttClient);
void MQTT_postData(float humidity,float temperature,int pm1,int pm25,int pm10,float O3);

void SDcard_init();
void SDcard_getData(float humidity,float temperature,int pm1,int pm25,int pm10,int O3ppb,float O3ppm,float O3ug ,int pm25_min, int maxpm25_max);
void SDcardScreen_splitStringData();
void SDcard_readFile();
void SDcard_saveDataFile();
void SD_runProgram();

void SHT_getData();
void SHT_init();

void TFLP01_getData();
void TFLP01_init();

void Screen_init();
void Screen_saveCalibData2SDcard();
void Screen_getDisplayData();
void Screen_displayData();
void Screen_displayCalibData();

void O3_init();
void O3_getData();

bool Button_isLongPressed();



//========================== Khai bao cac file code ========================

#include "config.h"
#include "SHT85Driver.h"
#include "TFLP01Driver.h"
#include "DS3231Driver.h"

#include "SDcardDriver.h"
#include "MQ131Driver.h"
#include "NextionDriver.h"
#include "ButtonDriver.h"
#include "MQTTConnection.h"


//==========================     SETUP       ========================

typedef void* TaskHandle_t;


TaskHandle_t WIFI_SmartConfig_Handle = NULL;
TaskHandle_t Sensors_getData_Handle = NULL;
TaskHandle_t Screen_display_Handle = NULL;
TaskHandle_t MQTT_sendData_Handle = NULL;
TaskHandle_t SD_writeData_Handle = NULL;


void SmartConfig_Task(void * parameters)
{
	// check button de smartConfig
	for(;;)
	{
		if (Button_isLongPressed())
		{
			uint8_t wifi_connectTrialCount_u8 = 0;
			while (!WiFi.smartConfigDone() && wifi_connectTrialCount_u8 < WIFI_MAX_CONNECT_TRIAL)
			{
				Serial.println(".");
				TFT_wifiStatus = WIFI_Status_et::WIFI_SCANNING;
				myNex.writeNum("dl.wifi.val", TFT_wifiStatus);
				wifi_connectTrialCount_u8++;
			}
		}
		vTaskDelay(TASK_DELAY);
	}
}


void Wifi_checkStatus_Task(void *parameters)
{
	for(;;)
	{
		if (WiFi.status() == wl_status_t::WL_CONNECTED)
		{
			TFT_wifiStatus = WIFI_Status_et::WIFI_CONNECTED;
		}
		else
		{
			TFT_wifiStatus = WIFI_Status_et::WIFI_DISCONNECT;
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
		
		SHT_getData();
		TFLP01_getData();
		DS3231_getData();
		vTaskDelay(TASK_DELAY);
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
		MQTT_postData(TFT_humidity_percent, TFT_temperature_C, TFT_pm1_u32, TFT_pm25_u32, TFT_pm10_u32, TFT_o3_ppb);
		mqttClient.loop();

		vTaskDelay(MQTT_TASK_DELAY);
	}
}

void SD_writeData_Task(void *parameters)
{
	for(;;)
	{
		SDcard_saveDataFile(TFT_humidity_percent, TFT_temperature_C, TFT_pm1_u32, TFT_pm25_u32, TFT_pm10_u32, TFT_o3_ppb, TFT_o3_ppm, TFT_o3_ug, pm25_min_u32, pm25_max_u32);
		SD_runProgram();

		vTaskDelay(SD_TASK_DELAY);
	}
}

void setup() {
	myNex.NextionListen();
	Serial.begin(SERIAL_DEBUG_BAUDRATE);
	pinMode(PIN_BUTTON_1, INPUT);
	Wire.begin(PIN_SDA_GPIO, PIN_SCL_GPIO, I2C_CLOCK_SPEED);
	WiFi.begin();
	WiFi.macAddress(MacAddress);


	// khoi tao cac cam bien

	Serial.println("Check Dusty Sensor.");
	TFLP01_init();
	delay(10);

	Serial.println("Check Temperature and Humidity Sensor.");
	SHT_init();
	delay(10);

	Serial.println("Check RTC Module.");
	DS3231_init();
	delay(10);

	Serial.println("Check Screen.");
	Screen_init();

#ifdef O3_SENSOR
	Serial.println("Check Ozone Sensor.");
	O3_init();
	delay(10);
#endif

///

#ifdef USING_MQTT
	MQTT_initClient(topic, espID, mqttClient);
	timeClient.begin();
#endif

///

#ifdef USING_SD_CARD
	Serial.println("Check SD");
	SDcard_init();
	delay(10);
	sprintf(nameFileCalib, "/calib-%d.txt", yearCalib_u32);
#endif

///
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



//==========================     LOOP       ========================

void loop() {

}
