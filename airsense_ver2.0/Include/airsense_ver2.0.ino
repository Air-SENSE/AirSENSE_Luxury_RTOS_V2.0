//========================== Khai bao thu vien ========================

#include <Arduino.h>
#include <string.h>
#include "Wire.h"
#include <SPI.h>
#include "WiFi.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>



//========================== Cac ham su dung ========================

void DS3231_Init();
void DS3231_GetData();

void MQTT_InitClient(char* _topic, char* _espID, PubSubClient& _mqttClient);
void MQTT_PostData(uint32_t, uint32_t, int, int, int, uint32_t);

void SDcard_Init();
void SDcard_GetData(uint32_t, uint32_t, int, int, int, int, uint32_t, uint32_t , int, int);
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


//========================== Khai bao cac file code ========================

#include "./config.h"
#include "./ButtonDriver.h"
#include "./SHT85Driver.h"
#include "./TFLP01Driver.h"
#include "./DS3231Driver.h"
#include "./MQTTConnection.h"
#include "./SDcardDriver.h"
#include "./MQ131Driver.h"
#include "./NextionDriver.h"
#include "./ButtonDriver.h"


//========================== 			Tasks		========================

TaskHandle_t WIFI_SmartConfig_Handle = NULL;
TaskHandle_t Sensors_getData_Handle = NULL;
TaskHandle_t Screen_Display_Handle = NULL;
TaskHandle_t MQTT_sendData_Handle = NULL;
TaskHandle_t SD_writeData_Handle = NULL;


void SmartConfig_Task(void * parameters)
{
	// check button de smartConfig
	for(;;)
	{
		if (Button_isLongPressed())
		{
			uint8_t wifi_ConnectTrialCount_u8 = 0;
			while (!WiFi.isSmartConfigDone() && wifi_ConnectTrialCount < WIFI_MAX_CONNECT_TRIAL)
			{
				Serial.println(".");
				TFT_wifiStatus = WIFI_Status_et::WIFI_SCANNING;
				myNex.writeNum("dl.wifi.val", TFT_WifiStatus);
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
		vTaskDelay(WIFI_DELAY);
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

void Screen_Display_Task(void *parameters) 
{
	for(;;)
	{
		Screen_DisplayData();
		vTaskDelay(TASK_DELAY);
	}
}

void MQTT_sendData_Task(void *parameters)
{
	for (;;)
	{
		MQTT_PostData(TFT_humi, TFT_temp, TFT_pm1, TFT_pm25, TFT_pm10, TFT_o3_ppb);
		mqttClient.loop();

		vTaskDelay(MQTT_TASKDELAY);
	}
}

void SD_writeData_Task(void *parameters)
{
	for(;;)
	{
		SDcard_SaveDataFile(TFT_humi, TFT_temp, TFT_pm1, TFT_pm25, TFT_pm10, TFT_o3_ppb, TFT_o3_ppm, TFT_o3_ug, min_pm25, max_pm25);
		runProgramWithSD();

		vTaskDelay(SD_TASKDELAY);
	}
}



//==========================     SETUP       ========================

void setup() {
	myNex.NextionListen();
	Serial.begin(SERIAL_DEBUG_BAUDRATE);
	pinMode(PIN_BUTTON_1, INPUT);
	Wire.begin(PIN_SDA_GPIO, PIN_SCL_GPIO, I2C_CLOCK_SPEED);
	WiFi.begin();
	WiFi.macAddress(MacAddress1);


	// khoi tao cac cam bien

	Serial.println("Check Dusty Sensor.");
	TFLP01_Init();
	delay(10);

	Serial.println("Check Temperature and Humidity Sensor.");
	SHT_Init();
	delay(10);

	Serial.println("Check RTC Module.");
	DS3231_Init();
	delay(10);

	Serial.println("Check Screen.");
	Screen_Init();

#ifdef O3_SENSOR
	Serial.println("Check Ozone Sensor.");
	O3_init();
	delay(10);
#endif

///

#ifdef USING_MQTT
	MQTT_InitClient(topic, espID, mqttClient);
	timeClient.begin();
#endif

///

#ifdef USING_SD_CARD
	Serial.println("Check SD");
	SDcard_Init();
	delay(10);
	sprintf(nameFileCalib, "/calib-%d.txt", yearCalib);
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


	xTaskCreatePinnedToCore(Wifi_Check_Status_Task,
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


	xTaskCreatePinnedToCore(Screen_Display_Task,
							"Screen dislay",
							STACK_SIZE,
							NULL,
							1,
							&Screen_Display_Handle,
							1 			// core 1
							);


}

//==========================     LOOP       ========================

void loop()
{
	// nothing
}
