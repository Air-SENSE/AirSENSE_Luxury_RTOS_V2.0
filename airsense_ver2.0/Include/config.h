#pragma once

//========================== DEFINE CHO TOAN BO CODE ========================
#define DEBUG_SERIAL
#define USING_SD_CARD
#define USING_MQTT

#define O3_SENSOR
#define O3_SENSOR_DF_ROBOT
#define O3_SENSOR_MQ131

//========================== DEFINE CHO MAN HINH ========================
#define SERIAL_DEBUG_BAUDRATE  115200
#define ERROR_READ_DISPLAY     777777

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
#define PIN_CS_SDCARD   5

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

uint32_t LastReconnectWIFI = 0;
uint32_t lastsenddatatoSD_MQTT = millis();

//========================== khai bao cac bien hien thi tren man hinh ========================

typedef enum 
{
    WIFI_DISCONNECT,
    WIFI_CONNECTED,
    WIFI_SCANNING
} WIFI_Status_et;

WIFI_Status_et TFT_wifiStatus = WIFI_Status_et::WIFI_DISCONNECT;

typedef enum
{
    SD_DISCONNECT,
    SD_CONNECTED
} SD_Status_et;

SD_Status_et  TFT_SDcard = SD_Status_et::SD_DISCONNECT;


char  TFT_string[10];

uint32_t  temp		  =  20;
uint32_t  humi		  =  60;
uint32_t  yearCalib   =  2021;
uint32_t  TFT_o3_ppb  =  0;
uint32_t  TFT_o3_ppm  =  0;
uint32_t  TFT_o3_ug   =  0;
uint32_t  min_o3_ppb  =  100;
uint32_t  min_o3_ppm  =  100;
uint32_t  min_o3_ug   =  100;
uint32_t  max_o3_ppb  =  0;
uint32_t  max_o3_ppm  =  0;
uint32_t  max_o3_ug   =  0;

//========================== khai bao de dung cho the nho ========================

const int SD_chipSelect = 5;
uint8_t MacAddress1[6];
char 	nameDevice1[12];

//========================== cac bien de calib gia tri ========================

extern int dataCalibInt   = 0;
extern int tempCalibInt   = 0;
extern int humiCalibInt   = 0;
extern int pm1CalibInt    = 0;
extern int pm10CalibInt   = 0;
extern int pm25CalibInt   = 0;
extern int tempCalibFloat = 0;
extern int humiCalibFloat = 0;
int getTemp = 0;

//========================== cac bien de doc gia tri tu man hinh ========================


int tempFromDisplay 	  = 0;
int humiFromDisplay 	  = 0;
int pm1FromDisplay		  = 0;
int pm10FromDisplay 	  = 0;
int pm25FromDisplay 	  = 0;
int tempFloatFromDisplay  = 0;
int humiFloatFromDisplay  = 0;

//==========================  DEFINE cho cac Task  ====================================


#define TASK_DELAY       ((TickType_t) 1000 / portTICK_PERIOD_MS)
#define WIFI_TASK_DELAY  ((TickType_t) WIFI_TIME_RECONNECT / portTICK_PERIOD_MS)
#define MQTT_TASK_DELAY  ((TickType_t) MQTT_TIME_SENDDATA / portTICK_PERIOD_MS)
#define SD_TASK_DELAY    ((TickType_t) SD_TIME_WRITEDATA / portTICK_PERIOD_MS)
#define STACK_SIZE      1024

//==========================  Khai bao thoi gian   ====================================

#define MQTT_SENDDATA_PERIOD	    (uint32_t)10000
#define SD_WRITEDATA_PERIOD		    (uint32_t)10000
#define WIFI_RECONNECT_PERIOD	    (uint32_t)60000
#define BUTTON_PRESSED_DURATION 	(uint32_t)4000
#define WIFI_MAX_CONNECT_TRIAL      (uint8_t)120

