#pragma once
#if (defined(USING_MQTT))

WiFiUDP ntpUDP;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 3600*7, 60000);

#define MQTT_SERVER_ADDRESS "188.68.48.86"
#define MQTT_SERVER_PORT  	1883U

#define ERROR_MQTT_INIT_FAILED		0x51
#define ERROR_MQTT_POST_DATA_FAILED 0X52

char topic[32];
char espID[10];


/**
 * @brief	ham de khoi tao ket noi mqtt
 * @param[in]	_topic - topic cua server tra ve
 * @param[in]	_espID - ID cua esp
 * @param[out]	_mqttClient - bien luu tru ket noi
 * @return ERROR_CODE
 */
uint32_t MQTT_initClient(char*			 _topic,
					 	 char*			 _espID,
					 	 PubSubClient& 	 _mqttClient,
						 struct connectionStatus _connecctionStatus)
{
	uint8_t espMacAddress[6];				// mang luu dia chi MAC
	WiFi.macAddress(espMacAddress);			// lay dia chi MAC cua WIFI
	uint32_t macAddressDecimal = (espMacAddress[3] << 16) + (espMacAddress[4] << 8) + espMacAddress[5];

	sprintf(_topic, "/AirSENSE v3_luxury/ESP32_%08d/", macAddressDecimal);
	sprintf(_espID, "%08d", macAddressDecimal);

	_mqttClient.setServer(MQTT_SERVER_ADDRESS, MQTT_SERVER_PORT);		// cai dat server voi dia chi ip va port
	
	if (!_mqttClient.connect(_espID))		// ket noi 
	{
		LOG_PRINT_INFORMATION("MQTT initialized failed!");
		return ERROR_MQTT_INIT_FAILED;
	}								

	LOG_PRINT_INFORMATION("Topic: %s.", _topic);
	LOG_PRINT_INFORMATION("ID: %s.", _espID);
	
	if(_mqttClient.connect(espID))
	{
		_connecctionStatus.mqttConnection = status_et::CONNECTED;
		LOG_PRINT_INFORMATION("MQTT initialized successfully!");
		return ERROR_NONE;
	} else {
		_connecctionStatus.mqttConnection = status_et::DISCONNECTED;
		LOG_PRINT_INFORMATION("MQTT initialized failed!");
		return ERROR_MQTT_INIT_FAILED;
	}
}


/**
 * @brief	ham de gui data len MQTT
 * 
 * @param[in] 	_sensorData_st: struct data sensor
 * @param[in]	_mqttClient: bien luu tru ket noi
 * @param[in]	_timeClient: bien lay thoi gian tu server "de.pool.ntp.org"
 * @param[in] 	_nameDevice: ten thiet bi
 * @return  ERROR_CODE
 */
uint32_t MQTT_postData(struct sensorData  _sensorData_st,
					   struct connectionStatus _connecctionStatus,
					   PubSubClient&  _mqttClient,
					   NTPClient&  _timeClient,
					   const char  *_nameDevice)
{
	if (_connecctionStatus.wifiStatus == status_et::CONNECTED)
	{
		if (_mqttClient.connected())
		{
			_connecctionStatus.mqttConnection = status_et::SENDING_DATA;
			_timeClient.update();
			uint32_t epochTime_u32 = _timeClient.getEpochTime();   	// lay thoi gian (tinh bang so giay)
			char message[256] = {0};

			// tao chuoi ki tu chua du lieu
			sprintf(message, 
					"{\"DATA\":{\"Temperature\":%.1f,\"Humidity\":%.1f,\"Pm1\":%u,\"Pm25\":%u,\"Pm10\":%u,\"Pm25_min\":%u,\"Pm25_max\":%u,\"O3ppb\":%u,\"O3ppm\":%.3f,\"O3ug\":%.1f,\"Time\":%u},\"NodeID\":\"%s\"}",
					_sensorData_st.temperature,
					_sensorData_st.humidity,
					_sensorData_st.pm1_u32,
					_sensorData_st.pm25_u32,
					_sensorData_st.pm10_u32,
					_sensorData_st.pm25_min_u32,
					_sensorData_st.pm25_max_u32,
					_sensorData_st.o3_ppb,
					_sensorData_st.o3_ppm,
					_sensorData_st.o3_ug,
					epochTime_u32,
					nameDevice);

			if (_mqttClient.publish(topic, message, true))	// kiem tra co gui dulieu len MQTT thanh cong
			{
				LOG_PRINT_INFORMATION("%s.", message);
				_connecctionStatus.mqttConnection = status_et::CONNECTED;
			}
			_mqttClient.loop();
			LOG_PRINT_INFORMATION("MQTT send data successfully!");
			return ERROR_NONE;
		}
		else
		{
			// _mqttClient.connect(espID);
			_connecctionStatus.mqttConnection = status_et::DISCONNECTED;
			LOG_PRINT_INFORMATION("MQTT post data failed!");
			//_mqttClient.subscribe("huuhuong");
			return ERROR_MQTT_POST_DATA_FAILED
		}
	}
}

#endif
