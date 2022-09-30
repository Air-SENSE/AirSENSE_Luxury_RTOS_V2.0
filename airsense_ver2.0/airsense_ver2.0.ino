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
#include <WiFiUdp.h>


//======================  Khai bao thoi gian cho  ===================

#define MQTT_Time_SendData		(uint32_t)10000
#define WIFI_Time_Reconnect		(uint32_t)60000
#define ButtonPress_Time 		(uint32_t)4000

//========================== Cac ham su dung ========================


void DS3231_Init();
void DS3231_GetData();

void MQTT_InitClient(char* _topic, char* _espID, PubSubClient& _mqttClient);
void MQTT_PostData(float hum,float tem,int pm1,int pm25,int pm10,float O3);

void SDcard_Init();
void SDcard_GetData(float hum,float tem,int pm1,int pm25,int pm10,int O3ppb,float O3ppm,float O3ug ,int minpm25, int maxpm25);
void SDcardScreen_SplitStringData();
void SDcard_ReadFile();
void SDcard_SaveDataFile();
void runProgramWithSD();

void SHT_GetData();
void SHT_Init();

void TFLP01_GetData();
void TFLP01_Init();

void Screen_Init();
void Screen_SaveCalibData2SDcard();
void Screen_GetDisplayData();
void Screen_DisplayData();
void Screen_DisplayCalibData();

void O3_Init();
void O3_GetData();

bool isLongPressButton();


//========================== Khai bao cac file code ========================

#include "./config.h"
#include "./SHT85Driver.h"
#include "./TFLP01Driver.h"
#include "./DS3231Driver.h"
#include "./MQTTConnection.h"
#include "./SDcardDriver.h"
#include "./MQ131Driver.h"
#include "./NextionDriver.h"


//========================== 			Tasks		========================

#define xDelay ((TickType_t) 1000 / portTICK_PERIOD_MS)
#define WIFI_xDelay ((TickType_t) WIFI_Time_Reconnect / portTICK_PERIOD_MS)
#define MQTT_xDelay ((TickType_t) MQTT_Time_SendData / portTICK_PERIOD_MS)


TaskHandle_t WIFI_handle = NULL;
TaskHandle_t Screen_display_handle = NULL;
TaskHandle_t MQTT_SendData_Handle = NULL;

void SmartConfig_Task(void * parameters)
{
	// check button de smartConfig
	for(;;)
	{
		if (isLongPressButton())
		{
			uint8_t count = 0;
			while (!WiFi.smartConfigDone() && count < 120)
			{
				Serial.println(".");
				TFT_wifi = 2;
				myNex.writeNum("dl.wifi.val",TFT_wifi);
				count++;
			}
		}
		vTaskDelay(xDelay);
	}
}


void Wifi_Check_Status_Task(void *parameters)
{
	for(;;)
	{
		if (WiFi.status() == WL_CONNECTED)
		{
			TFT_wifi = 1;
		}
		else
		{
			TFT_wifi = 0;
		}
		vTaskDelay(WIFI_xDelay);
	}
}

void GetData_Task(void *parameters)
{
	for(;;)
	{
		#ifdef O3_SENSOR
			O3_GetData();
		#endif
		
		SHT_GetData();
		TFLP01_GetData();
		DS3231_GetData();
		vTaskDelay(xDelay);
	}
}

void Screen_Display_Task(void *parameters) 
{
	for(;;)
	{
		Screen_DisplayData();
		vTaskDelay(xDelay);
	}
}

void SendDatatoSD_MQTT_Task(void *parameters)
{
	for(;;)
	{
		#ifdef USING_SD_CARD
			SDcard_SaveDataFile(TFT_humi, TFT_temp, TFT_pm1, TFT_pm25, TFT_pm10, TFT_o3_ppb, TFT_o3_ppm, TFT_o3_ug, min_pm25, max_pm25);
			runProgramWithSD();
		#endif

		#ifdef USING_MQTT
			MQTT_PostData(TFT_humi,TFT_temp,TFT_pm1,TFT_pm25,TFT_pm10,TFT_o3_ppb);
			mqttClient.loop();
		#endif

		vTaskDelay(MQTT_xDelay);
	}
}



//==========================     SETUP       ========================

void setup() {
	myNex.NextionListen();
	// Serial.begin(SERIAL_DEBUG_BAUDRATE);
	pinMode(PIN_BUTTON_1,	INPUT);
	Wire.begin(PIN_SDA_GPIO, PIN_SCL_GPIO, I2C_CLOCK_SPEED);
	WiFi.begin();
	WiFi.macAddress(MacAddress1);

#ifdef USING_MQTT
	MQTT_InitClient(topic, espID, mqttClient);
	timeClient.begin();
#endif
	// khoi tao cac cam bien
#ifdef O3_SENSOR
	Serial.println("Check Ozone Sensor");
	O3_init();
	delay(10);
#endif

	// Serial.println("Check Dusty  Sensor");
	// TFLP01_Init();
	// delay(10);

	// Serial.println("Check Temperature and Humidity Sensor");
	// SHT_Init();
	// delay(10);

	// Serial.println("Check RTC Module");
	// DS3231_Init();
	// delay(10);

	// Serial.println("Check Screen");
	// Screen_Init();


#ifdef USING_SD_CARD
	Serial.println("Check SD");
	SDcard_Init();
	delay(10);
#endif

#ifdef USING_SD_CARD
	//   luu file text theo nam
	sprintf(nameFileCalib, "/calib-%d.txt", yearCalib);
#endif

	// core 0
	xTaskCreatePinnedToCore(SmartConfig_Task, "Smart Config", 1000, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(Wifi_Check_Status_Task, "WIFI status", 1000, NULL, 1, NULL, 0);
	xTaskCreatePinnedToCore(SendDatatoSD_MQTT_Task, "Send data", 1000, NULL, 1, NULL, 0);

	// core 1
	xTaskCreatePinnedToCore(GetData_Task, "Get Data", 1000, NULL, 1, NULL, 1);
	xTaskCreatePinnedToCore(Screen_Display_Task, "Screen dislay", 1000, NULL, 1, NULL, 1);
// viet tai lieu cho RTOS

}

//==========================     LOOP       ========================

void loop()
{

}

bool isLongPressButton()
{
	if (millis() - lastPressButton > LongTimePressButton && digitalRead(PIN_BUTTON_1) == 0)
	{
		return true;
	}
	else if (digitalRead(PIN_BUTTON_1) == 1)
	{
		lastPressButton = millis();
	}
	return false;
}
