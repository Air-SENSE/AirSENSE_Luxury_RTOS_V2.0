#pragma once
//========================== DEFINE CHO TOAN BO CODE ========================
#define DEBUG_SERIAL
#define USING_SD_CARD
#define USING_MQTT

#define O3_SENSOR
#define O3_SENSOR_DF_ROBOT
#define O3_SENSOR_MQ131
#define O3_GETDATA_PERIOD  (uint32_t)5000

#define PIN_MQ131_POWER      2
#define PIN_MQ131_SENSOR     4  

//========================== DEFINE CHO MAN HINH ========================
#define SERIAL_DEBUG_BAUDRATE   115200
#define ERROR_READ_DISPLAY      777777
#define NEXTION_BAUDRATE        9600
#define EEPROM_SIZE             512

//========================== DEFINE CHO MAN HINH ========================

#define PIN_TX_TFLP01   17
#define PIN_RX_TFLP01   16

//========================== DEFINE CHO MAN HINH ========================

#define PIN_TX_TFT      33
#define PIN_RX_TFT      25

//========================== DEFINE CHO MAN HINH ========================

#define PIN_NUM_MISO    21
#define PIN_NUM_MOSI    19
#define PIN_NUM_CLK     18
#define PIN_CS_SD_CARD  5
//========================== DEFINE CHO MAN HINH ========================

#define PIN_SDA_GPIO    26
#define PIN_SCL_GPIO    27
#define I2C_CLOCK_SPEED 100000ul

//========================== DEFINE CHO MAN HINH ========================

#define PIN_NEO_PIXEL   23

//========================== DEFINE CHO MAN HINH ========================

#define PIN_BUTTON_1    35
#define PIN_BUTTON_2    34
// #define outputA         33
// #define outputB         25
// #define switch          32

//========================== DEFINE CHO MAN HINH ========================

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 3600*7, 60000);

//========================== DEFINE CHO MAN HINH ========================

typedef enum 
{
    WIFI_DISCONNECT,
    WIFI_CONNECTED,
    WIFI_SCANNING
} WIFI_Status_et;

WIFI_Status_et TFT_wifiStatus = WIFI_Status_et::WIFI_DISCONNECT;

uint32_t lastreconnectwifi = 0;
uint32_t lastsenddatatoSD_MQTT = millis();

//========================== khai bao cac bien hien thi tren man hinh ========================

char  TFT_string[10];

float      temp		        = 20.5;
float      humi		        = 60;
uint32_t   yearCalib_u32    = 2021;
int        TFT_o3_ppb       = 0;
float      TFT_o3_ppm       = 0;
float      TFT_o3_ug        = 0;
int        min_o3_ppb       = 100;
float      min_o3_ppm       = 100;
float      min_o3_ug        = 100;
int        max_o3_ppb       = 0;
float      max_o3_ppm       = 0;
float      max_o3_ug        = 0;

//========================== khai bao de dung cho the nho ========================

//uint8_t MacAddress[6];
char 	nameDevice[12];

//========================== cac bien de calib gia tri ========================

 uint32_t data_calibInt_u32           = 0;
 uint16_t temperature_calibInt_u16    = 0;
 uint16_t humidity_calibInt_u16       = 0;
 uint32_t pm1_calibInt_u32            = 0;
 uint32_t pm10_calibInt_u32           = 0;
 uint32_t pm25_calibInt_u32           = 0;
 uint16_t tempperature_calibFloat_u16 = 0;
 uint16_t humidity_calibFloat_u16     = 0;

//========================== cac bien de doc gia tri tu man hinh ========================

uint16_t  dipslay_temperatureInt_u16 	= 0;
uint16_t  display_humidityInt_u16 	    = 0;
uint16_t  display_pm1_u16		        = 0;
uint16_t  display_pm10_u16 	            = 0;
uint16_t  display_pm25_u16	            = 0;
uint16_t  display_temperatureFloat_u16  = 0;
uint16_t  display_humidityFloat_u16     = 0;


#define MQTT_SENDDATA_PERIOD	    (uint32_t)10000
#define SD_WRITEDATA_PERIOD		    (uint32_t)10000
#define WIFI_RECONNECT_PERIOD	    (uint32_t)6000
#define BUTTON_PRESSED_DURATION 	(uint32_t)4000
#define WIFI_MAX_CONNECT_TRIAL      (uint8_t)120


#define TASK_DELAY       ((TickType_t) 1000 / portTICK_PERIOD_MS)
#define WIFI_TASK_DELAY  ((TickType_t) WIFI_RECONNECT_PERIOD / portTICK_PERIOD_MS)
#define MQTT_TASK_DELAY  ((TickType_t) MQTT_SENDDATA_PERIOD / portTICK_PERIOD_MS)
#define SD_TASK_DELAY    ((TickType_t) SD_WRITEDATA_PERIOD / portTICK_PERIOD_MS)
#define STACK_SIZE       1024
