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

char fileNameCalib[] = "calib.txt";			// file chua cac gia tri calib
char nameFileSaveData[10];					// ten file luu du lieu cua sensor theo tung ngay


/**
 * @brief	ham de khoi tao the nho
 *
 * @return  None
 */
bool SDcard_init()
{
	SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD);
	pinMode(SS, OUTPUT);

	if (SD.begin(PIN_CS_SD_CARD))
	{
		TFT_SDStatus = SD_Status_et::SD_CONNECTED;
		Serial.println("SD init done.");
		return false;
	}
	else
	{
		TFT_SDStatus = SD_Status_et::SD_DISCONNECT;
		Serial.println("SD init false.");
		return true;		
	}
}

/**
 * @brief	split gia tri du lieu tu the nho
 *
 * @return  None
 */
void splitStringData(char *arrayData_pc)
{
#ifdef DEBUG_SERIAL
	Serial.print("Char_arr in split: ");
	Serial.println(arrayData_pc);
#endif

	char temperature[30] 		= "";
	char humidityi[30] 			= "";
	char pm1[30] 				= "";
	char pm10[30] 				= "";
	char pm25[30] 				= "";
	char temperatureFloat[30] 	= "";
	char humidityiFloat[30] 	= "";

	// split string
	char *token_ps = NULL;			// token_ps: con trỏ trỏ đến chuỗi sau khi đã cắt token
	
	token_ps = strtok(arrayData_pc, "|");
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
	if (token_ps)		strcpy(temperatureFloat, token_ps);
	tempperature_calibFloat_u16 = atoi(temperatureFloat);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(humidityiFloat, token_ps);
	humidity_calibFloat_u16 = atoi(humidityiFloat);

#ifdef DEBUG_SERIAL
	Serial.println("After Split: ");
	Serial.println(temperature_calibInt_u16);
	Serial.println(humidity_calibInt_u16);
	Serial.println(pm1_calibInt_u32);
	Serial.println(pm10_calibInt_u32);
	Serial.println(pm25_calibInt_u32);
	Serial.println(tempperature_calibFloat_u16);
	Serial.println(humidity_calibFloat_u16);
	Serial.println("*************");
#endif
}

/**
 * @brief lay ten file luu du lieu theo nam,thang,ngay
 * 
 * @return char* 
 */
char * getNameFile()
{
	strcpy(nameFileSaveData, realTime.now().toString("YY-MMM-DD"));
	return nameFileSaveData;
}


/**
 * @brief	Doc file tu trong the nho
 *
 * @return  None
 */
void SDcard_readFile()
{
	char arrayData[30];
	myFile3 = SD.open(fileNameCalib, FILE_READ);
	String finalString = "";

	while (myFile3.available())
	{
		finalString += (char)myFile3.read();
	}
	strcpy(arrayData, finalString.c_str());

	#ifdef DEBUG_SERIAL
		Serial.println("----- Reading file -----");
		Serial.print("Data:");
		Serial.println(arrayData);
	#endif

	splitStringData(arrayData);
	myFile3.close();
}


/**
 * @brief	ham de luu gia tri vao the nho
 * @param	humidity - gia tri do am
 * @param	temperature - gia tri nhiet do
 * @param	pm1 - pm1.0
 * @param	pm25 - pm2.5
 * @param	pm10 - pm10
 * @param	O3ppb - 03 don vi do ppb
 * @param	O3ppm - 03 don vi do ppm
 * @param	O3ug - 03 don vi do ug
 * @param	pm25_min - muc min cua pm25
 * @param	pm25_max - muc max cua pm25
 * @return  None
 */
void SDcard_saveDataToFile( float 	humidity,
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
	
	char dateTime_string[21];
	strcpy(dateTime_string, realTime.now().toString("YYYY-MMM-DD,hh:mm:ss"));		// lay chuoi ki tu thoi gian

	if (realTime.now().isValid())		// kiem tra 
	{
		myFile = SD.open(nameFileSaveData, FILE_APPEND);
		if (myFile)
		{
			char message[256] = {0};
			sprintf(message,"%12s,%s,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%.3f,%.1f\n",
							nameDevice,
							dateTime_string,
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
			myFile.println(message);
			myFile.close();
		}
		else
		{
			if (SD.begin(PIN_CS_SD_CARD, SPI))
				TFT_SDStatus = SD_Status_et::SD_CONNECTED;
			else
				TFT_SDStatus = SD_Status_et::SD_DISCONNECT;
		}
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("-------------- Loi lay ngay sai ---------------");
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
	// doc cac gia tri calib tu man hinh
	Screen_getCalibData();

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
		// luu gia tri tu calib vao the sd
		Screen_saveCalibDataToSDcard();
		// hien thi cac gia tri calib len man hinh
		Screen_displayCalibData();
	}
}

#endif
