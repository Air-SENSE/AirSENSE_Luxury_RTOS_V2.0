/**
 * @file MQTTConnection.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
char espID[10];


WiFiUDP ntpUDP;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 3600*7, 60000);

#define MQTT_SERVER_IP_ADDRESS "188.68.48.86"
#define MQTT_SERVER_PORT  	1883U

#define ERROR_MQTT_INIT_FAILED		(0x51)
#define ERROR_MQTT_POST_DATA_FAILED (0X52)

char topic[32];

#if (defined(USING_MQTT))

/**
 * @brief	ham de khoi tao ket noi mqtt
 * @param[in]	_topic  topic cua server tra ve
 * @param[in]	_espID  ID cua esp
 * @param[out]	_mqttClient  bien luu tru ket noi
 * @return ERROR_CODE
 */
ERROR_CODE MQTT_initClient( char*			 		_topic,
					 	 	char*			 		_espID,
					 	 	PubSubClient& 	 		_mqttClient,
						 	struct connectionStatus *_connectionStatus)
{
	uint8_t espMacAddress[6];				// mang luu dia chi MAC
	WiFi.macAddress(espMacAddress);			// lay dia chi MAC cua WIFI
	uint32_t macAddressDecimal = (espMacAddress[3] << 16) + (espMacAddress[4] << 8) + espMacAddress[5];

	sprintf(_topic, "/AirSENSE v3_luxury/ESP32_%8d/", macAddressDecimal);
	sprintf(_espID, "%8d", macAddressDecimal);

	_mqttClient.setServer(MQTT_SERVER_IP_ADDRESS, MQTT_SERVER_PORT);		// cai dat server voi dia chi ip va port

	if(_mqttClient.connect(_espID))
	{
		LOG_PRINT_INFORMATION("Topic: %s.", _topic);
		LOG_PRINT_INFORMATION("ID: %s.", _espID);
		_mqttClient.subscribe(_topic);
		_connectionStatus->mqttConnection = status_et::CONNECTED;
		LOG_PRINT_INFORMATION("MQTT initialized successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("MQTT initialized failed!");
		_connectionStatus->mqttConnection = status_et::DISCONNECTED;
		LOG_PRINT_INFORMATION("MQTT initialized failed!");
		return ERROR_MQTT_INIT_FAILED;
	}
}



/**
 * @brief	ham de gui data len MQTT
 * 
 * @param[in] 	_message : chuoi du lieu
 * @param[in]	_mqttClient: bien luu tru ket noi
 * @param[in]	_timeClient: bien lay thoi gian tu server "de.pool.ntp.org"
 * @param[in] 	_nameDevice: ten thiet bi
 * @return  ERROR_CODE
 */
ERROR_CODE MQTT_postData(const char *_message,
					     struct connectionStatus 	*_connectionStatus,
					     PubSubClient&  			_mqttClient)
{
	if (_connectionStatus->wifiStatus == status_et::CONNECTED)
	{
		if (_mqttClient.connected())
		{
			_connectionStatus->mqttConnection = status_et::SENDING_DATA;

			if (_mqttClient.publish(topic, _message, true))	// kiem tra co gui dulieu len MQTT thanh cong
			{
				LOG_PRINT_INFORMATION("%s.", _message);
				_connectionStatus->mqttConnection = status_et::CONNECTED;
				_mqttClient.loop();
				LOG_PRINT_INFORMATION("MQTT send data successfully!");
				return ERROR_NONE;
			} else {
				_connectionStatus->mqttConnection = status_et::DISCONNECTED;
				LOG_PRINT_INFORMATION("MQTT post data failed!");
				return ERROR_MQTT_POST_DATA_FAILED;
			}

		} else {
			_connectionStatus->mqttConnection = status_et::DISCONNECTED;
			LOG_PRINT_INFORMATION("MQTT post data failed!");
			return ERROR_MQTT_POST_DATA_FAILED;
		}
	} else {
		LOG_PRINT_INFORMATION("WIFI  disconnected. MQTT post data failed!");
		return ERROR_MQTT_POST_DATA_FAILED;
	}
}

#endif
