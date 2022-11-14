/**
 * @file file.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdarg.h>
#include <SD.h>

const char fileNameCalib[] = "calib.txt";			// file chua cac gia tri calib
char nameFileSaveData[12];							// ten file luu du lieu cua sensor theo tung ngay


#define ERROR_SD_CARD_INIT_FAILED 		0x31
#define ERROR_SD_CARD_READ_FILE_FAILED 	0x32
#define ERROR_SD_CARD_WRITE_FILE_FAILED 0x33
#define ERROR_SD_CARD_FILE_NOT_FOUND 	0x34


/**
 * @brief Initialize SD Card
 * 
 * @param[in] _pinSCK:  SCK pin
 * @param[in] _pinMISO: MISO pin
 * @param[in] _pinMOSI: MOSI pin
 * @param[in] _pinCS:   ChipSelect pin
 * @param[in] _connecctionStatus: pointer to struct status connection
 * @return ERROR_CODE
 */
ERROR_CODE SDcard_init(uint8_t _pinSCK,
					   uint8_t _pinMISO,
					   uint8_t _pinMOSI,
					   uint8_t _pinCS,
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
 * @brief	Doc file tu trong the nho
 * 
 * @param[in]  	fileName_string                 : ten file
 * @param[in]	numberOfRequestedByte_u32		: so byte muon doc
 * @param[out]  fileContent_string				: chuoi chua noi dung file
 * @param[out]  numberOfReadByte_u32			: so byte doc duoc
 * 
 * @return  ERROR_CODE
 */
ERROR_CODE SDcard_readFile( const char *fileName_string,
							char *fileContent_string,
							const uint32_t numberOfRequestedByte_u32,
							uint32_t *numberOfReadByte_u32)
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
	else{
		return ERROR_SD_CARD_READ_FILE_FAILED;	 
	}
}


/**
 * @brief 
 * 
 * @param[in]   _nameFile: name file save calibdata
 * @param[in]   _format: format store data in file
 * @param[out]  __VA_ARGS__ List argument (pointer)
 * @return ERROR_CODE 
 */
ERROR_CODE SDcard_readCalibDataFromFile(struct connectionStatus *_connectStatus,
										const char *_nameFile,
										const char *_format,
										...)
{
	if (_connectStatus->sdCardStatus == status_et::CONNECTED)
	{
		File readFile;
		readFile = SD.open(_nameFile, FILE_READ);
		if (readFile)			// kiem tra tinh trang mo file co thanh cong
		{
			_connectStatus->sdCardStatus = status_et::READING_DATA;
			char fileContent_string[35];

			for (size_t i = 0; (i < 34) && (readFile.available()); i++)
			{
				fileContent_string[i] = (char)readFile.read();
			}

			va_list argumentsList;
			va_start(argumentsList, _format);
			vsscanf(fileContent_string, _format, argumentsList);
			va_end(argumentsList);

			readFile.close();
			_connectStatus->sdCardStatus = status_et::CONNECTED
			LOG_PRINT_INFORMATION("SD card read calibrate data successfully!");
			return ERROR_NONE;
		} else {
			LOG_PRINT_INFORMATION("File not found!");
			LOG_PRINT_INFORMATION("SD card read calibrate data failed!");
			return ERROR_SD_CARD_FILE_NOT_FOUND;
		}
	} else {
		LOG_PRINT_INFORMATION("SD card disconnected!");
		LOG_PRINT_INFORMATION("SD card read calibrate data failed!");
		return ERROR_SD_CARD_READ_FILE_FAILED;
	}
}


/**
 * @brief Save sensor data to SDcard
 * 
 * @param[in] fileContent_string: string save to file
 * @param[in] _sensorData_st: struct sensor data
 * @param[in] _connecctionStatus: pointer to struct store all connection status
 * @return ERROR_CODE 
 */
ERROR_CODE SDcard_saveStringDataToFile( struct connectionStatus *_connectStatus,
										const char *fileContent_string,
								 		struct sensorData  _sensorData_st,
					   	   		 		struct connectionStatus *_connecctionStatus)
{
	if (_connectStatus->sdCardStatus == status_et::CONNECTED)
	{
		File writeFile;

		writeFile = SD.open(nameFileSaveData, FILE_APPEND);		// mo file de ghi du lieu
		if (writeFile)											// kiem tra trang thai mo file co thanh cong
		{
			_connectStatus->sdCardStatus = status_et::WRITING_DATA;
			writeFile.println(fileContent_string);		// ghi chuoi ki tu chua du lieu vao file
			writeFile.close();							// dong file
			_connectStatus->sdCardStatus = status_et::CONNECTED;
			LOG_PRINT_INFORMATION("%s", fileContent_string);
			LOG_PRINT_INFORMATION("SD card write sensor data to file successfully!");
			return ERROR_NONE;
		}
		else
		{
			LOG_PRINT_INFORMATION("Can't open file to write!");
			LOG_PRINT_INFORMATION("SD card write sensor data to file failed!");
			return ERROR_SD_CARD_FILE_NOT_FOUND;
		}
	} else {
		LOG_PRINT_INFORMATION("SD card disconnected!");
		LOG_PRINT_INFORMATION("SD card write data failed!");
		return ERROR_SD_CARD_READ_FILE_FAILED;
	}
}

#if(0)
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

