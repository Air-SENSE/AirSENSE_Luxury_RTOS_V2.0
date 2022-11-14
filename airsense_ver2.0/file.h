#pragma once
#if(defined(USING_SDCARD))

#include <SD.h>

const char fileNameCalib[] = "calib.txt";			// file chua cac gia tri calib
char nameFileSaveData[12];							// ten file luu du lieu cua sensor theo tung ngay

#define ERROR_SD_CARD_INIT_FAILED 0x31
#define ERROR_SD_CARD_READ_FILE_FAILED 0x32
#define ERROR_SD_CARD_WRITE_FILE_FAILED 0x33
#define ERROR_SD_CARD_FILE_NOT_FOUND 0x34


/**
 * @brief Initialize SD Card
 * 
 * @param[in] _pinSCK: SCK pin
 * @param[in] _pinMISO: MISO pin
 * @param[in] _pinMOSI: MOSI pin
 * @param[in] _pinCS: ChipSelect pin
 * @param[in] _connecctionStatus: pointer to struct status connection
 * @return ERROR_CODE
 */
uint32_t SDcard_init(uint8_t _pinSCK = PIN_NUM_CLK,
				 uint8_t _pinMISO = PIN_NUM_MISO,
				 uint8_t _pinMOSI = PIN_NUM_MOSI,
				 uint8_t _pinCS = PIN_CS_SD_CARD,
				 struct connectionStatus *_connecctionStatus)
{
	SPI.begin(_pinSCK, _pinMISO, _pinMOSI, _pinCS);
	pinMode(SS, OUTPUT);

	if (SD.begin(_pinCS))
	{
		_connecctionStatus->sdCardStatus = status_et::CONNECTED;
		LOG_PRINT_INFORMATION("SD init success.");
		return ERROR_NONE;
	}
	else
	{
		_connecctionStatus->sdCardStatus = status_et::DISCONNECTED;
		LOG_PRINT_INFORMATION("SD init false.");
		return ERROR_SD_CARD_INIT_FAILED;		
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
 * @param 
 * @param 
 * @return  None
 */
void splitStringData(char *arrayData_pc, struct calibData *_calibData)
{
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
	_calibData->temperature_calibInt_u32 = atoi(temperature);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(humidityi, token_ps);
	_calibData->humidity_calibInt_u32 = atoi(humidityi);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm1, token_ps);
	_calibData->pm1_calibInt_u32 = atoi(pm1);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm10, token_ps);
	_calibData->pm10_calibInt_u32 = atoi(pm10);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(pm25, token_ps);
	_calibData->pm25_calibInt_u32 = atoi(pm25);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(temperatureFloat, token_ps);
	_calibData->temperature_calibFloat_u32 = atoi(temperatureFloat);

	token_ps = strtok(NULL, "|");
	if (token_ps)		strcpy(humidityiFloat, token_ps);
	_calibData->humidity_calibFloat_u32 = atoi(humidityiFloat);

	LOG_PRINT_INFORMATION("After Split: ");
	LOG_PRINT_INFORMATION(_calibData->temperature_calibInt_u32);
	LOG_PRINT_INFORMATION(_calibData->humidity_calibInt_u32);
	LOG_PRINT_INFORMATION(_calibData->pm1_calibInt_u32);
	LOG_PRINT_INFORMATION(_calibData->pm10_calibInt_u32);
	LOG_PRINT_INFORMATION(_calibData->pm25_calibInt_u32);
	LOG_PRINT_INFORMATION(_calibData->temperature_calibFloat_u32);
	LOG_PRINT_INFORMATION(_calibData->humidity_calibFloat_u32);
	LOG_PRINT_INFORMATION("*****************");
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
		numberOfReadByte_u32 = 0;
		while (readFile.available() && ((*numberOfReadByte_u32) < numberOfRequestedByte_u32) )
		{
			fileContent_string[(*numberOfReadByte_u32)] = (char)readFile.read();
			(*numberOfReadByte_u32)++;
		}
		readFile.close();
		return ERROR_NONE;
	}	
	else
		return ERROR_SD_CARD_READ_FILE_FAILED;	 
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
void SDcard_saveDataSensorToFile(const char* _nameDevice,
								 RTC_DS3231 &_realTime,
								 struct sensorData  _sensorData_st,
					   	   		 struct connectionStatus *_connecctionStatus)
{
	File writeFile;

	if (getNameFileFollowDateTime())							// kiem tra ngay thang co hop le
	{
		writeFile = SD.open(nameFileSaveData, FILE_APPEND);		// mo file de ghi du lieu
		if (writeFile)											// kiem tra trang thai mo file co thanh cong
		{
			char dateTime_string[25];
			DS3231_getStringDateTime(_realTime, "YYYY-MMM-DD,hh:mm:ss", dateTime_string);


			// tao chuoi ki tu chua du lieu theo cau truc
			char message[256] = {0};
			sprintf(message,"%s,%s,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%.3f,%.1f\n",
							_nameDevice,
							dateTime_string,
							_sensorData_st.temperature,
							_sensorData_st.humidity,
							_sensorData_st.pm1_u32,
							_sensorData_st.pm25_u32,
							_sensorData_st.pm10_u32,
							_sensorData_st.pm25_min_u32,
							_sensorData_st.pm25_max_u32,
							_sensorData_st.o3_ppb,
							_sensorData_st.o3_ppm,
							_sensorData_st.o3_ug,);

			LOG_PRINT_INFORMATION("%s", message);		// in chuoi ki tu chua du lieu ra Serial
			writeFile.println(message);		// ghi chuoi ki tu chua du lieu vao file
			writeFile.close();				// dong file

			LOG_PRINT_INFORMATION("SD card write sensor data to file successfully!");
			return ERROR_NONE;
		}
		else
		{
			return ERROR_SD_CARD_WRITE_FILE_FAILED;
		}
	}
	else
	{
		LOG_PRINT_INFORMATION("DateTime incorrect!");
		return ERROR_SD_CARD_WRITE_FILE_FAILED;
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
