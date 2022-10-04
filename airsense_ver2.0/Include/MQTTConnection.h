#pragma once
#ifndef _MQTTCONNECTION_H_
#define _MQTTCONNECTION_H_

#ifdef USING_MQTT

WiFiClient espClient;
PubSubClient mqttClient(espClient);

//const char mqttServerAddress[15] = "23.89.159.119";
const char mqttServerAddress[15] = "188.68.48.86";
const uint16_t mqttServerPort = 1883;

char topic[32];
char espID[10];

char nameDevice[12];
uint8_t MacAddress[6];
uint32_t lastSenddatatoMQTT = millis();


void MQTT_initClient(char*, char*, PubSubClient&);
void MQTT_postData(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#endif
#endif