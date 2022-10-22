#pragma once
#if (defined(USING_MQTT))

WiFiUDP ntpUDP;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 3600*7, 60000);

//const char mqttServerAddress[15] = "23.89.159.119";
const char mqttServerAddress[15] = "188.68.48.86";
const uint16_t mqttServerPort = 1883;

char topic[32];
char espID[10];

uint8_t MacAddress[6];


typedef enum 
{
    WIFI_DISCONNECT,
    WIFI_CONNECTED,
    WIFI_SCANNING
} WIFI_Status_et;

WIFI_Status_et TFT_wifiStatus = WIFI_Status_et::WIFI_DISCONNECT;		// bien trang thai ket noi WIFI


/**
 * @brief	ham de khoi tao ket noi mqtt
 * @param	_topic - topic cua server tra ve
 * @param	_espID - ID cua esp
 * @param	_mqttClient - bien luu tru ket noi
 * @return None
 */
void MQTT_initClient(char*			 _topic,
					 char*			 _espID,
					 PubSubClient& 	_mqttClient)
{
	uint8_t espMacAddress[6];				// mang luu dia chi MAC
	WiFi.macAddress(espMacAddress);			// lay dia chi MAC cua WIFI
	uint32_t macAddressDecimal = (espMacAddress[3] << 16) + (espMacAddress[4] << 8) + espMacAddress[5];

	sprintf(_topic, "/AirSENSE v3_luxury/ESP32_%08d/", macAddressDecimal);
	sprintf(_espID, "%08d", macAddressDecimal);

	_mqttClient.setServer(mqttServerAddress, mqttServerPort);		// cai dat server voi dia chi ip va port
	_mqttClient.connect(_espID);									// ket noi 

	Serial.println(_topic);
	Serial.println(_espID);
	mqttClient.connect(espID);
}

/**
 * @brief	ham de gui data len MQTT
 * @param	humidity - gia tri do am
 * @param	temperature - gia tri nhiet do
 * @param	pm1_u32 - pm1.0
 * @param	pm25_u32 - pm2.5
 * @param	pm10_u32 - pm10
 * @param	O3ppb_u32 - 03 don vi do ppb
 * @param	O3ppm - 03 don vi do ppm
 * @param	O3ug - 03 don vi do ug
 * @param	pm25_min_u32 - nong do pm25 min
 * @param	pm25_max_u32 - nong do pm25 max
 * @return  None
 */
void MQTT_postData ( float 		humidity,
					 float 		temperature,
					 uint32_t   pm1_u32,
					 uint32_t   pm25_u32,
					 uint32_t 	pm10_u32,
					 uint32_t 	O3ppb_u32,
					 float 		O3ppm,
					 float 		O3ug,
					 uint32_t 	pm25_min_u32,
					 uint32_t 	pm25_max_u32 )
{
	if (WiFi.status() == WL_CONNECTED)
	{
		if (mqttClient.connected())
		{
			timeClient.update();
			uint32_t epochTime_u32 = timeClient.getEpochTime();   	// lay thoi gian (tinh bang so giay)
			char message[256] = {0};

			// tao chuoi ki tu chua du lieu
			sprintf(message, 
					"{\"DATA\":{\"O3ppb\":%u,\"O3ppm\":%.3f,\"O3ug\":%.1f,\"Temperature\":%.1f,\"Humidity\":%.1f,\"Pm1\":%u,\"Pm25\":%u,\"Pm10\":%u,\"Pm25_min\":%u,\"Pm25_max\":%u,\"Time\":%u},\"NodeID\":\"%s\"}",
					O3ppb_u32,
					O3ppm,
					O3ug,
					temperature,
					humidity,
					pm1_u32,
					pm25_u32,
					pm10_u32,
					pm25_min_u32,
					pm25_max_u32,
					epochTime_u32,
					nameDevice );

			if (mqttClient.publish(topic, message, true))	// kiem tra co gui dulieu len MQTT thanh cong
			{
#ifdef	DEBUG_SERIAL
			Serial.println(message); 
#endif
			}
			mqttClient.loop();
		}
		else 
		{
			mqttClient.connect(espID);
#ifdef	DEBUG_SERIAL
			Serial.println(" - mqtt reconnect ");
			mqttClient.subscribe("huuhuong");
#endif
		}
	}
}

#endif
