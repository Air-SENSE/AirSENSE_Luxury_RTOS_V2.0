#pragma once
#if (defined(USING_MQTT))

WiFiClient espClient;
PubSubClient mqttClient(espClient);

//const char mqttServerAddress[15] = "23.89.159.119";
const char mqttServerAddress[15] = "188.68.48.86";
const uint16_t mqttServerPort = 1883;

char topic[32];
char espID[10];

//char nameDevice[12];
uint8_t MacAddress[6];
uint32_t lastSenddatatoMQTT = millis();



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
	uint8_t espMacAddress[6];
	WiFi.macAddress(espMacAddress);
	uint32_t macAddressDecimal = (espMacAddress[3] << 16) + (espMacAddress[4] << 8) + espMacAddress[5];
	sprintf(_topic, "/AirSENSE v3_luxury/ESP32_%08d/", macAddressDecimal);
	sprintf(_espID, "%08d", macAddressDecimal);
	_mqttClient.setServer(mqttServerAddress, mqttServerPort);
	_mqttClient.connect(_espID);
	Serial.println(_topic);
	Serial.println(_espID);
	mqttClient.connect(espID);
}

/**
 * @brief	ham de gui data len MQTT
 * @param	humidity - do am
 * @param	temperature -  nhiet do
 * @param	pm1 - pm1.0
 * @param	pm25 - pm2.5
 * @param	pm10 - pm10
 * @param	03 - 03
 * @return  None
 */
void MQTT_postData (float 	humidity,
					float 	temperature,
					int 	pm1,
					int 	pm25,
					int 	pm10,
					float 	O3)
{
	if (WiFi.status() == WL_CONNECTED)
	{
	if (mqttClient.connected())
	{
		timeClient.update();
		uint32_t epochTime_u32 = timeClient.getEpochTime();   
		char message[256] = {0};

		sprintf(message, 
				"{\"DATA\":{\"O3\":%.1f,\"Humidity\":%.1f,\"Pm1\":%d,\"Pm10\":%d,\"Pm2p5\":%d,\"Time\":%d,\"Temperature\":%.1f},\"NodeID\":\"%s\"}",
				O3,
				humidity,
				pm1,
				pm10,
				pm25,
				epochTime_u32,
				temperature,
				nameDevice );

		if (mqttClient.publish(topic, message, true))
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
