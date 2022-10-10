#pragma once
#if(defined(USING_SD_CARD))

#include <SD.h>

typedef enum
{
	SD_DISCONNECT,
	SD_CONNECTED
} SD_Status_et;

SD_Status_et TFT_SDStatus = SD_Status_et::SD_DISCONNECT;

File myFile;
File myFile2;
File myFile3;
File myFile4;
const int chipSelect = 5;

char char_array[30];
char fileNameCalib[] = "calib.txt";

char nameFileCalib1[16];
char nameFileCalib[16];

/**
 * @brief	ham de khoi tao the nho
 *
 * @return  None
 */
void SDcard_init()
{
SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD);
pinMode(SS, OUTPUT);
if (!SD.begin(PIN_CS_SD_CARD))
{
	TFT_SDStatus = SD_Status_et::SD_DISCONNECT;
	Serial.println("SD init false.");
}
else
{
	TFT_SDStatus = SD_Status_et::SD_CONNECTED;
	Serial.println("SD init done.");
}
}

/**
 * @brief	split gia tri du lieu tu the nho
 *
 * @return  None
 */
void Screen_splitStringData()
{
#ifdef DEBUG_SERIAL
	Serial.print("Char_arr in split: ");
	Serial.println(char_array);
#endif

	char temperature[30] 	= "";
	char humidityi[30] 		= "";
	char pm1[30] 			= "";
	char pm10[30] 			= "";
	char pm25[30] 			= "";
	char temperatureFl[30] 	= "";
	char humidityiFl[30] 	= "";

	// split string
	char *token_ps = NULL;			// token_ps: con trỏ trỏ đên chuỗi sau khi đã cắt token
	
	token_ps = strtok(char_array, "|");
	if (token_ps)		strcpy(temperature, token_ps);
	temperature_calibInt_u16 = atoi(temperature);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(humidityi, token_ps);
	humidity_calibInt_u16 = atoi(humidityi);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm1, token_ps);
	pm1_calibInt_u32 = atoi(pm1);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm10, token_ps);
	pm10_calibInt_u32 = atoi(pm10);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm25, token_ps);
	pm25_calibInt_u32 = atoi(pm25);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(temperatureFl, token_ps);
	tempperature_calibFloat_u16 = atoi(temperatureFl);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(humidityiFl, token_ps);
	humidity_calibFloat_u16 = atoi(humidityiFl);

#ifdef DEBUG_SERIAL
	Serial.println("After Split: ");
	Serial.println(temperature_calibInt_u16);
	Serial.println(humidity_calibInt_u16);
	Serial.println(pm1_calibInt_u32);
	Serial.println(pm10_calibInt_u32);
	Serial.println(pm25_calibInt_u32);
	Serial.println(tempperature_calibFloat_u16);
	Serial.println(humidity_calibFloat_u16);
	Serial.println("*********");
#endif
}

/**
 * @brief	Doc file tu trong the nho
 *
 * @return  None
 */
void SDcard_readFile()
{
	myFile3 = SD.open(nameFileCalib, FILE_READ);
	String finalString = "";

	while (myFile3.available())
	{
		finalString += (char)myFile3.read();
	}
	strcpy(char_array, finalString.c_str());
	#ifdef DEBUG_SERIAL
	Serial.println("--- Reading file ---");
	Serial.print("char_array:");
	Serial.println(char_array);
	#endif
	Screen_splitStringData();
	myFile3.close();
}

/**
 * @brief	ham de luu gia tri vao the nho
 * @param	humidity - gia tri do am
 * @param	temperature - gia tri nhiet do
 * @param	pm1 - pm1.0
 * @param	pm25 - pm2.5
 * @param	pm10 - pm10
 * @param	O3ppb - 03
 * @param	O3ppm - 03
 * @param	O3ug - 03
 * @param	minpm25 - muc min cua pm25
 * @param	maxpm25 - muc max cua pm25
 * @return  None
 */
void SDcard_saveDataFile(float 	humidity,
						 float 	temperature,
						 int   	pm1,
						 int   	pm25,
						 int 	pm10,
						 int 	O3ppb,
						 float 	O3ppm,
						 float 	O3ug,
						 int 	pm25_min,
						 int 	pm25_max )
{
	DateTime now = rtc.now();
	int getyear = now.year();
	uint8_t getmonth = now.month();
	uint8_t getday = now.day();
	uint8_t gethour = now.hour();
	uint8_t getminute = now.minute();
	uint8_t getsecond = now.second();
	uint32_t epochTime = now.unixtime();
	uint8_t y = getyear % 2000;

	char filename[16];
	if (getday < 10 && getmonth < 10)
		sprintf(filename, "/%d-0%d-0%d.txt", y, getmonth, getday);
	if (getday < 10 && getmonth >= 10)
		sprintf(filename, "/%d-%d-0%d.txt", y, getmonth, getday);
	if (getday >= 10 && getmonth < 10)
		sprintf(filename, "/%d-0%d-%d.txt", y, getmonth, getday);
	if (getday >= 10 && getmonth >= 10)
		sprintf(filename, "/%d-%d-%d.txt", y, getmonth, getday);

	if (0 <= y && y <= 22 && 0 < getmonth && getmonth <= 12 && 0 < getday && getday <= 31)
	{
		myFile = SD.open(filename, FILE_WRITE);
		if (myFile)
		{
		char message[256] = {0};

		sprintf(message, "%12s,%4d/%d/%d,%d:%d:%d,%d,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%.3f,%.1f",
						nameDevice,
						getyear,
						getmonth,
						getday,
						gethour,
						getminute,
						getsecond,
						epochTime,
						temperature,
						humidity,
						pm1,
						pm25,
						pm10,
						pm25_min,
						pm25_max,
						O3ppb,
						O3ppm,
						O3ug );


	#ifdef DEBUG_SERIAL
		Serial.println(message);
	#endif
		TFT_SDStatus = SD_Status_et::SD_CONNECTED;
		myFile.println(message);
		myFile.close();
		}
		else
		{
	#ifdef DEBUG_SERIAL
		// Serial.println("reconnect SD");
	#endif
		if (!SD.begin(PIN_CS_SD_CARD, SPI))
			TFT_SDStatus = SD_Status_et::SD_DISCONNECT;
		else
			TFT_SDStatus = SD_Status_et::SD_CONNECTED;
		}
	}
	else
	{
	#ifdef DEBUG_SERIAL
		Serial.println("--------------Loi lay ngay sai: " + String(filename) + "---------------");
		// rtc.begin();
	#endif
	}
}


/**
 * @brief	Chuong trinh hoat dong cua the nho
 *
 * @return  None
 */
void SD_runProgram()
{
	// doc gia tri tu man hinh
	Screen_getDisplayData();

	// neu man hinh moi khoi tao
	if (dipslay_temperatureInt_u16 == 0 && display_humidityInt_u16 == 0 && display_pm1_u16 == 0 && display_pm10_u16 == 0 && display_pm25_u16 == 0)
	{
		// doc gia tri tu the nho
		SDcard_readFile();
		// gui gia tri len man hinh calib
		Screen_displayCalibData();
	}
	else
	{
		// doc gia tri tu calib vao the sd
		Screen_saveCalibData2SDcard();
		// doc nguoc lai the sd
		SDcard_readFile();
		// hien thi lai len man hinh
		Screen_displayCalibData();
	}
}

#endif
