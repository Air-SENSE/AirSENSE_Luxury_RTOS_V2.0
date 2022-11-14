#pragma once
#include <string>
#include "string.h"
#include "config.h"
#include "EasyNextionLibrary.h"
#include <EEPROM.h>

#define DATA_IS_CORRECT					ERROR_NONE
#define DISPLAY_DATA_SUCCESSFULLY		ERROR_NONE
#define SCREEN_UPDATE_STATUS_SUCCESS 	ERROR_NONE
#define SCREEN_UPDATE_DATETIME_SUCCESS 	ERROR_NONE

#define ERROR_SCREEN_INIT_FAILED 				  0x41
#define ERROR_GET_CALIB_DATA_FROM_SCREEN_FAILED   0x42
#define ERROR_DATA_INCORRECT 					  0x43
#define ERROR_SCREEN_DISPLAY_DATA_FAILED 		  0x44
#define ERROR_SCREEN_UPDATE_STATUS_FAILED 		  0x45
#define ERROR_SCREEN_UPDATE_DATETIME_FAILED		  0x46
#define ERROR_SCREEN_DISPLAY_sensorData_FAILED 	  0x47


EasyNex myNex(SCREEN_SERIAL_PORT);			// khai bao doi tuong man hinh Nextion ket noi Serial (Serial)


/**
 * @brief Initialize Nextion screen
 * 
 * @param _stream Serial port name
 * @param baudRate_u32 Baudrate value
 * @param romSize_u32 Sizeof EEPROM
 * 
 * @return ERROR_NONE on success
 */
uint32_t Screen_init(Stream& _stream, const uint32_t baudRate_u32 = 9600, const uint32_t romSize_u32 = 512)
{
	myNex.begin(baudRate_u32);		// khoi dong man hinh Nextion 

	if (_stream.available() && EEPROM.begin(romSize_u32);)		// kiem tra ket noi voi man hinh va khoi dong EEPROM cua man hinh Nextion
	{
		LOG_PRINT_INFORMATION("Nextion screen initialize successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("Nextion screen initialize failed!");
		return ERROR_SCREEN_INIT_FAILED;
	}
}



/**
 * @brief Read calibration data form address of a components' numeric attribute on Nextion.
 * 
 * @param[in] objectName: paremetter use to read the value of a components' numeric attribute on Nextion
 * In every component's numeric attribute (value, bco color, pco color...etc).
 * @param[out] calibValue: pointer to variable stores calibration data.
 * 
 * @return 	ERROR_NONE on success.
 */
extern uint32_t Screen_getDataFromTextBox(const char *objectName, uint32_t *calibValue)
{
	uint32_t calibValueTemp_u32 = myNex.readNumber(objectName);
	if (calibValueTemp_u32 == ERROR_READ_DISPLAY)
	{
		*calibValue = calibValueTemp_u32;
		LOG_PRINT_INFORMATION("Read calibration data form address \"%s\" successfully!", objectName);
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("Read calibration data form address \"%s\" failed!", objectName);
		return ERROR_GET_CALIB_DATA_FROM_SCREEN_FAILED;
	}
}


/**
 * @brief	Read all calibration data input from screen by user
 *
 * @param[in] _calibData: pointer to calibration data struct
 * 
 * @return  ERRROR_NONE on success
 */
uint32_t Screen_getCalibDataFromUser(struct calibData *_calibData)
{
	if (Screen_getDataFromTextBox("calibEdit.n0.val", &(_calibData->temperature_calibInt_u32  )) &&
		Screen_getDataFromTextBox("calibEdit.n1.val", &(_calibData->humidity_calibInt_u32     )) &&
		Screen_getDataFromTextBox("calibEdit.n2.val", &(_calibData->pm1_calibInt_u32          )) &&
		Screen_getDataFromTextBox("calibEdit.n3.val", &(_calibData->pm10_calibInt_u32         )) &&
		Screen_getDataFromTextBox("calibEdit.n4.val", &(_calibData->pm25_calibInt_u32         )) &&
		Screen_getDataFromTextBox("calibEdit.n5.val", &(_calibData->temperature_calibFloat_u32)) &&
		Screen_getDataFromTextBox("calibEdit.n6.val", &(_calibData->humidity_calibFloat_u32   )) )
	{
#ifdef DEBUG_SERIAL
		LOG_PRINT_INFORMATION(" %u.", _calibData->temperature_calibInt_u16    );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->humidity_calibInt_u16       );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->pm1_calibInt_u32            );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->pm10_calibInt_u32           );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->pm25_calibInt_u32           );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->temperature_calibFloat_u16  );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION(" %u.", _calibData->humidity_calibFloat_u16     );		// print calibration data to Serial port
		LOG_PRINT_INFORMATION("--------------------------");
#endif
		LOG_PRINT_NOTIFICATION("Read all calibration data successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_ERROR("Read all calibration data failed!");
		return ERROR_GET_CALIB_DATA_FROM_SCREEN_FAILED;
	}
}


/**
 * @brief Check calibration data
 * 
 * @param[in] _calibData: pointer to calibration data struct
 * 
 * @return true neu cac gia tri hop le
 * @return false neu cac gia tri khong hop le
 */
uint32_t checkDataValid(struct calibData *_calibData)
{
	if (( &(_calibData->temperature_calibInt_u32 > 1000	)) 	||
 		( &(_calibData->humidity_calibInt_32 	 > 1000	))	||
 		( &(_calibData->pm1_calibInt_u32 		 > 1000	)) 	||
 		( &(_calibData->pm10_calibInt_u32 		 > 1000	)) 	||
 		( &(_calibData->pm25_calibInt_u32 		 > 1000	))	)
	{
		LOG_PRINT_NOTICATION("Calibration data is correct!");
		return DATA_IS_CORRECT;
	} else {
		LOG_PRINT_INFORMATION("Calibration data is incorrect!");
		return ERROR_DATA_INCORRECT;
	}
}



/**
 * @brief Display calibration data to screen
 * 
 * @param[in] _calibData: pointer to calibration data struct
 * 
 * @return DISPLAY_DATA_SUCCESSFULLY on display success
 */
uint32_t Screen_displayCalibData(struct calibData *_calibData)
{
	if (SCREEN_SERIAL_PORT.available())
	{
		myNex.writeNum("dl.n4.val", &(_calibData->temperature_calibInt_u32   ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n5.val", &(_calibData->humidity_calibInt_u32      ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n6.val", &(_calibData->pm1_calibInt_u32           ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n7.val", &(_calibData->pm10_calibInt_u32          ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n8.val", &(_calibData->pm25_calibInt_u32          ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n7.val", &(_calibData->temperature_calibFloat_u32 ));		// Write calibration data to address on Nextion screen
		myNex.writeNum("dl.n8.val", &(_calibData->humidity_calibFloat_u32    ));		// Write calibration data to address on Nextion screen
		LOG_PRINT_INFORMATION("Display calibration data on screen successfully!");
		return DISPLAY_DATA_SUCCESSFULLY;
	}  else {
		LOG_PRINT_INFORMATION("Display calibration data on screen failed!");
		return ERROR_SCREEN_DISPLAY_DATA_FAILED;
	}
}


/**
 * @brief Update status for Wifi connection and SD card connection 
 * 
 * @param[in] _connectionStatus_st pointer to struct connectionStatus
 * @return ERROR_CODE 
 */
uint32_t Screen_updateStatus(struct connectionStatus *_connectionStatus_st)
{
	if (SCREEN_SERIAL_PORT.available())
	{
		myNex.writeNum("dl.wifi.val", _connectionStatus_st->wifiStatus);		// cap nhat trang thai wifi
		myNex.writeNum("dl.sd.val", _connectionStatus_st->sdCardStatus);			// cap nhat trang thai the SD
		LOG_PRINT_INFORMATION("Update status successfully!");
		return SCREEN_UPDATE_STATUS_SUCCESS;
	} else
	{
		LOF_PRINT_INFORMATION("Update status failed!");
		return ERROR_SCREEN_UPDATE_STATUS_FAILED;
	}
}


/**
 * @brief Update current datetime 
 * 
 * @param[in] time Curent datetime object
 * @return ERROR_CODE 
 */
uint32_t Screen_updateCurrentDateTime(DateTime time)
{
	if (time.isValid())				// kiem tra thoi gian co hop le
	{
		myNex.writeStr("dl.time.txt", String(time.toString("hh:mm DD-MMM-YY")));		// in thoi gian ra man hinh neu thoi gian hop le
		LOG_PRINT_INFORMATION("Update datetime successfully!");
		return ERROR_NONE;
	} else {
		myNex.writeStr("dl.time.txt", "??:?? Na-NaN-NaN");								// in ra chuoi thoi gian loi neu thoi gian khong hop le
		LOG_PRINT_INFORMATION("Update datetime failed!");
		return ERROR_SCREEN_UPDATE_DATETIME_FAILED;
	}
}


/**
 * @brief Display data to screen
 * 
 * @param _sensorData_st pointer to struct sensorData
 * @return ERROR_CODE 
 */
uint32_t Screen_displaysensorData(struct sensorData *_sensorData_st)
{
	if (SCREEN_SERIAL_PORT.available())
	{
		myNex.writeStr("dl.temc.txt"   , String(_sensorData_st->temperature, 1U));
		myNex.writeStr("dl.hum.txt"    , String(_sensorData_st->humidity, 1U));

		myNex.writeNum("dl.nppb.val"   , _sensorData_st->o3_ppb);					// ghi gia tri O3 thoe don vi ppm ra man hinh 
		myNex.writeStr("dl.sppb.txt"   , String(_sensorData_st->o3_ppb, 10));		// ghi 

		myNex.writeStr("dl.sug.txt"    , String(_sensorData_st->o3_ug, 1U));
		myNex.writeStr("dl.sppm.txt"   , String((_sensorData_st->o3_ppb/1000.0), 3U));
		myNex.writeStr("dl.sminppb.txt", String(_sensorData_st->o3_ppb_min, 10));
		myNex.writeStr("dl.sminug.txt" , String(_sensorData_st->o3_ug_min, 1U));
		myNex.writeStr("dl.sminppm.txt", String((_sensorData_st->o3_ppb_min/1000.0), 3U));
		myNex.writeStr("dl.smaxppb.txt", String(_sensorData_st->o3_ppb_max, 10));
		myNex.writeStr("dl.smaxug.txt" , String(_sensorData_st->o3_ug_max, 1U));
		myNex.writeStr("dl.smaxppm.txt", String((_sensorData_st->o3_ppb_max/1000.0), 3U));

		myNex.writeNum("dl.npm25.val"  , _sensorData_st->pm25_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh
		myNex.writeNum("dl.npm1.val"   , _sensorData_st->pm1_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh
		myNex.writeNum("dl.npm10.val"  , _sensorData_st->pm10_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh

		myNex.writeStr("dl.spm1.txt"   , String((_sensorData_st->pm1_u32, 10)));	// ghi vao thanh phan text cua dia chi bien tren man hinh
		myNex.writeStr("dl.spm10.txt"  , String((_sensorData_st->pm10_u32, 10)));	// ghi vao thanh phan text cua dia chi bien tren man hinh
		myNex.writeStr("dl.spm25.txt"  , String((_sensorData_st->pm25_u32, 10)));	// ghi vao thanh phan text cua dia chi bien tren man hinh
		myNex.writeStr("dl.maxpm25.txt", String((_sensorData_st->pm25_max_u32, 10)));	// ghi vao thanh phan text cua dia chi bien tren man hinh
		myNex.writeStr("dl.minpm25.txt", String((_sensorData_st->pm25_min_u32, 10)));	// ghi vao thanh phan text cua dia chi bien tren man hinh

		LOG_PRINT_INFORMATION("Display data from sensor successfully!");
		return ERROR_NONE;
	} else {
		LOG_PRINT_INFORMATION("Display data from sensor failed!");
		return ERROR_SCREEN_DISPLAY_sensorData_FAILED;
	}
}

