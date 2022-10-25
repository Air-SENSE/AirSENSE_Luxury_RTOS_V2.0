#pragma once
#if(defined(USING_SD_CARD))

#include <SD.h>

typedef enum
{
	SD_DISCONNECT,
	SD_CONNECTED
} SD_Status_et;

SD_Status_et TFT_SDStatus = SD_Status_et::SD_DISCONNECT;


char fileNameCalib[] = "calib.txt";			// file chua cac gia tri calib
char nameFileSaveData[12];					// ten file luu du lieu cua sensor theo tung ngay

/**
 * @brief	ham de khoi tao the nho
 *
 * @return  TRUE: initialize successfully
 */
bool SDcard_init()
{
	SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD);
	pinMode(SS, OUTPUT);

	if (SD.begin(PIN_CS_SD_CARD))
	{
		TFT_SDStatus = SD_Status_et::SD_CONNECTED;
		LOG_PRINT_INFORMATION("SD init done.");
		return true;
	}
	else
	{
		TFT_SDStatus = SD_Status_et::SD_DISCONNECT;
		LOG_PRINT_INFORMATION("SD init false.");
		return false;		
	}
}


/**
 * @brief lay ten file luu du lieu theo nam,thang,ngay
 * @ Input data:
 * @ output data: file name stored in nameFileSaveData
 * @ parameter: 
 * @return bool
 */
bool getNameFileFollowDateTime()
{
	if(realTime.now().isValid()) 
	{
		strcpy(nameFileSaveData, (char*)realTime.now().toString("YY-MMM-DD"));		// lay ten file theo ngay/thang/nam
		return true;
	} else {
		strcpy(nameFileSaveData, "Na-NaN-NaN");
		return false;
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
	temperature_calibFloat_u16 = atoi(temperatureFloat);

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
	Serial.println(temperature_calibFloat_u16);
	Serial.println(humidity_calibFloat_u16);
	Serial.println("*************");
#endif
}


/**
 * @brief	Doc file tu trong the nho
 * @input: fileNameCalib
 * @output: buffer_string
 * @return  None
 */
void SDcard_readFile()
{
	File readFile;
	readFile = SD.open(fileNameCalib, FILE_READ);
	char dataSD_string[256] = {0};     
	if (readFile)			// kiem tra tinh trang mo file co thanh cong
	{
		String buffer_string = "";
		
		while (readFile.available())
		{
			buffer_string += (char)readFile.read();
		}
		strcpy(dataSD_string, buffer_string.c_str());
	}

	splitStringData(dataSD_string);
	readFile.close();
}

/**
 * @brief	Doc file tu trong the nho
 * @input:  char *fileName_string                  	: ten file
 * 			uint32_t numberOfRequestedByte_u32		: so byte muon doc
 * @output: char * fileContent_string				: chuoi chua noi dung file
 * 			uint32_t *numberOfReadByte_u32			: so byte doc duoc
 * @parameter: None
 * @return  error code
 */
uint16_t SDcard_readFile( char *fileName_string, char * fileContent_string, const uint32_t numberOfRequestedByte_u32, uint32_t *numberOfReadByte_u32)
{
	File readFile;
	readFile = SD.open(fileNameCalib, FILE_READ);
	if (readFile)			// kiem tra tinh trang mo file co thanh cong
	{
		String buffer_string = "";
		numberOfReadByte_u32=0;
		while (readFile.available() && ((*numberOfReadByte_u32)<numberOfRequestedByte_u32) )
		{
			fileContent_string[(*numberOfReadByte_u32)] = (char)readFile.read();
			(*numberOfReadByte_u32)++;
		}
		readFile.close();
		return ERROR_NONE;
	}	
	else
		return ERROR_FILE_NOT_FOUND;	 
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
	File writeFile;
	if (getNameFileFollowDateTime())							// kiem tra ngay thang co hop le
	{
		writeFile = SD.open(nameFileSaveData, FILE_APPEND);		// mo file de ghi du lieu
		if (writeFile)											// kiem tra trang thai mo file co thanh cong
		{
			char dateTime_string[21];
			strcpy(dateTime_string, realTime.now().toString("YYYY-MMM-DD,hh:mm:ss"));		// lay chuoi ki tu thoi gian hien tai

			// tao chuoi ki tu chua du lieu theo cau truc
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
			Serial.println(message);		// in chuoi ki tu chua du lieu ra Serial
#endif
			writeFile.println(message);		// ghi chuoi ki tu chua du lieu vao file
			writeFile.close();				// dong file
		}
		else
		{
			// if (SD.begin(PIN_CS_SD_CARD, SPI))					// ket noi lai voi the SD
			// 	TFT_SDStatus = SD_Status_et::SD_CONNECTED;		// cap nhat trang thai the nho
			// else
			// 	TFT_SDStatus = SD_Status_et::SD_DISCONNECT;		// cap nhat trang thai the nho

			// KIEM TRA TINH TRANG KHOI TAO THE SD
			LOG_PRINT_ASSERT_INFOR2(SD.begin(PIN_CS_SD_CARD, SPI),
									"SD init done.",
									"SD init false.");
		}
	}
	else
	{
#ifdef DEBUG_SERIAL
		Serial.println("-------------- Loi ngay sai ---------------");
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

	// kiem tra neu nguoi dung co nhap du lieu calib tren man hinh
	if ( (temperature_calibInt_u16 	== 0) &&
		 (humidity_calibInt_u16    	== 0) &&
		 (pm1_calibInt_u32 		  	== 0) &&
		 (pm10_calibInt_u32 		== 0) &&
		 (pm25_calibInt_u32 		== 0) )
	{
		// doc gia tri tu the nho
		SDcard_readFile();
		// gui gia tri len man hinh calib
		Screen_displayCalibData();
	}
	else
	{
		// luu gia tri calib nhap tu man hinh vao the sd
		Screen_saveCalibDataToSDcard();
		// hien thi cac gia tri calib len man hinh
		Screen_displayCalibData();
	}
}

#endif
