/************************************************************
**    TFT_o3_ppb_u32(int)  TFT_o3_ppm(float)  TFT_o3_ug(int)  **    
**    min_o3_ppb_u32(int)  min_o3_ppm(float)  min_o3_ug(int)  **
**    max_o3_ppb_u32(int)  max_o3_ppm(float)  max_o3_ug(int)  **
**    TFT_temp(uint8_t)            TFT_wifi               **
**    TFT_humi(uint8_t)            TFT_SDcard             **
**    TFT_temp_F(uint16_t)         Rx: pin 33             **
**    TFT_pm25(int)                Tx: pin 25             **
**    TFT_pm10(int)                                       **
**    TFT_time(string)                                    **
************************************************************/

/* Khai bao thu vien man hinh Nextion */
#pragma once
#include <string>
#include "string.h"
#include "config.h"
#include "EasyNextionLibrary.h"
#include <EEPROM.h>

EasyNex myNex(Serial);			// khia bao doi tuong man hinh Nextion ket noi Serial (Serial1)


/**
 * @brief	Khoi tao man hinh Nextion
 *
 * @return  None
 */
void Screen_init()
{
	myNex.begin(NEXTION_BAUDRATE);		// khoi dong man hinh Nextion 
	EEPROM.begin(EEPROM_SIZE);			// khoi dong EEPROM cua man hinh Nextion
}


/**
 * @brief Lay gia tri Calib của objectName tu man hinh
 * 
 * @param objectName: paremetter use to read the value of a components' numeric attribute on Nextion
 * In every component's numeric attribute (value, bco color, pco color...etc)
 * 
 * @return uint16_t 
 */
uint16_t getCalibData(const char *objectName)
{
	uint16_t calibValue = myNex.readNumber(objectName);
	return (calibValue == ERROR_READ_DISPLAY) ? 0 : calibValue;
}


/**
 * @brief	Doc du lieu tu man hinh va luu vao cac bien
 *
 * @return  None
 */
void Screen_getCalibData()
{
	temperature_calibInt_u16     =  getCalibData("calibEdit.n0.val");		// lay gia tri tu dia chi bien tren man hinh
	humidity_calibInt_u16        =  getCalibData("calibEdit.n1.val");		// lay gia tri tu dia chi bien tren man hinh
	pm1_calibInt_u32             =  getCalibData("calibEdit.n2.val");		// lay gia tri tu dia chi bien tren man hinh
	pm10_calibInt_u32            =  getCalibData("calibEdit.n3.val");		// lay gia tri tu dia chi bien tren man hinh
	pm25_calibInt_u32            =  getCalibData("calibEdit.n4.val");		// lay gia tri tu dia chi bien tren man hinh
	tempperature_calibFloat_u16  =  getCalibData("calibEdit.n5.val");		// lay gia tri tu dia chi bien tren man hinh
	humidity_calibFloat_u16      =  getCalibData("calibEdit.n6.val");		// lay gia tri tu dia chi bien tren man hinh

// In cac du lieu tu man hinh ra Serial
#ifdef DEBUG_SERIAL
	Serial.println(temperature_calibInt_u16    );		// in cac gia tri calib ra Serial
	Serial.println(humidity_calibInt_u16       );		// in cac gia tri calib ra Serial
	Serial.println(pm1_calibInt_u32            );		// in cac gia tri calib ra Serial
	Serial.println(pm10_calibInt_u32           );		// in cac gia tri calib ra Serial
	Serial.println(pm25_calibInt_u32           );		// in cac gia tri calib ra Serial
	Serial.println(tempperature_calibFloat_u16 );		// in cac gia tri calib ra Serial
	Serial.println(humidity_calibFloat_u16     );		// in cac gia tri calib ra Serial
	Serial.println("--------------");
#endif
}


/**
 * @brief kiem tra cac gia tri calib
 * 
 * @return true neu cac gia tri hop le
 * @return false neu cac gia tri khong hop le
 */
bool checkDataValid()
{
	return  ((temperature_calibInt_u16 < -100) 	|| (temperature_calibInt_u16 > 1000) ||
			 (humidity_calibInt_u16 < -100)		|| (humidity_calibInt_u16 > 1000)	 ||
			 (pm1_calibInt_u32 < -100)			|| (pm1_calibInt_u32 > 1000) 		 ||
			 (pm10_calibInt_u32 < -100)			|| (pm10_calibInt_u32 > 1000) 		 ||
			 (pm25_calibInt_u32 < -100) 		|| (pm25_calibInt_u32 > 1000));
}


/**
 * @brief	Luu tru cac gia tri da calib tu man hinh vao the nho
 *
 * @return  None
 */
void Screen_saveCalibDataToSDcard()
{
	if (checkDataValid())
	{
#ifdef DEBUG_SERIAL
		Serial.println("----- *** Don't write to SD card *** ----");			// in ra Serail
#endif
	}
	else
	{
		File writeFile;
		writeFile = SD.open(fileNameCalib, FILE_APPEND);
		if(writeFile)		// kiem tra mo file co thanh cong
		{
			// tao chuoi string theo cau truc: temperature|humidity|PM1.0|PM2.5|PM10|temperature_float|humidity_float
			char message[256] = {0};
			sprintf(message,"%u|%u|%u|%u|%u|%u|%u\n", temperature_calibInt_u16,   
													  humidity_calibInt_u16,  
													  pm1_calibInt_u32,
													  pm10_calibInt_u32,
													  pm25_calibInt_u32,
													  tempperature_calibFloat_u16,
													  humidity_calibFloat_u16 );

#ifdef DEBUG_SERIAL
			Serial.print("Message: ");
			Serial.println(message);							// in du lieu duoc tao ben tren ra Serial
#endif
			TFT_SDStatus = SD_Status_et::SD_CONNECTED;			// cap nhat trang tahi the nho
			writeFile.println(message);        					// ghi du lieu ra the nho
			writeFile.close();										// dong the nho
		}
		else
		{
			Serial.println("Reconnect SD.");
			SDcard_init();										// khoi dong lai the nho
		}
	}
}



/**
 * @brief	Doc cac gia tri calib tu the nho len man hinh
 *
 * @return  None
 */
void Screen_displayCalibData()
{
	//myNex.writeNum("dl.n9.val", data_calibInt_u32           );
	myNex.writeNum("dl.n4.val", temperature_calibInt_u16    );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n5.val", humidity_calibInt_u16       );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n6.val", pm1_calibInt_u32            );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n7.val", pm10_calibInt_u32           );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n8.val", pm25_calibInt_u32           );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n7.val", tempperature_calibFloat_u16 );		// lay gia tri calib tren man hinh vao bien
	myNex.writeNum("dl.n8.val", humidity_calibFloat_u16     );		// lay gia tri calib tren man hinh vao bien
}


/**
 * @brief	ghi cac gia tri len man hinh
 *
 * @return  None
 */
void Screen_displayData()
{
	myNex.writeNum("dl.wifi.val", TFT_wifiStatus);		// cap nhat trang thai wifi
	myNex.writeNum("dl.sd.val", TFT_SDStatus);			// cap nhat trang thai the SD

	if (realTime.now().isValid())		// kiem tra thoi gian co hop le
	{
		myNex.writeStr("dl.time.txt", realTime.now().toString("hh:mm DD-MMM-YY")); 		// in thoi gian ra man hinh neu thoi gian hop le
	} else {
		myNex.writeStr("dl.time.txt", "??:?? Na-NaN-NaN");								// in ra chuoi thoi gian loi neu thoi gian khong hop le
	}
	
	myNex.writeStr("dl.temc.txt"   , String(TFT_temperature_C, 1U));
	myNex.writeStr("dl.hum.txt"    , String(TFT_humidity_percent, 1U));
	myNex.writeStr("dl.temf.txt"   , String(TFT_temperature_F, 1U));

	myNex.writeNum("dl.nppb.val"   , TFT_o3_ppb_u32);					// ghi gia tri O3 thoe don vi ppm ra man hinh 
	myNex.writeStr("dl.sppb.txt"   , String(TFT_o3_ppb_u32, 10));		// ghi 

	myNex.writeStr("dl.sug.txt"    , String(TFT_o3_ug, 1U));
	myNex.writeStr("dl.sppm.txt"   , String((TFT_o3_ppb_u32/1000.0), 3U));
	myNex.writeStr("dl.sminppb.txt", String(min_o3_ppb_u32, 10));
	myNex.writeStr("dl.sminug.txt" , String(min_o3_ug, 1U));
	myNex.writeStr("dl.sminppm.txt", String((min_o3_ppb_u32/1000.0), 3U));
	myNex.writeStr("dl.smaxppb.txt", String(max_o3_ppb_u32, 10));
	myNex.writeStr("dl.smaxug.txt" , String(max_o3_ug, 1U));
	myNex.writeStr("dl.smaxppm.txt", String((max_o3_ppb_u32/1000.0), 3U));

	myNex.writeNum("dl.npm25.val"  , TFT_pm25_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh
	myNex.writeNum("dl.npm1.val"   , TFT_pm1_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh
	myNex.writeNum("dl.npm10.val"  , TFT_pm10_u32);					// in gia tri bui PMx ra dia chi bien tren man hinh

	myNex.writeStr("dl.spm1.txt"   , String(TFT_pm1_u32, 10) );	// ghi vao thanh phan text cua dia chi bien tren man hinh
	myNex.writeStr("dl.spm10.txt"  , String(TFT_pm10_u32, 10));	// ghi vao thanh phan text cua dia chi bien tren man hinh
	myNex.writeStr("dl.spm25.txt"  , String(TFT_pm25_u32, 10));	// ghi vao thanh phan text cua dia chi bien tren man hinh
	myNex.writeStr("dl.maxpm25.txt", String(pm25_max_u32, 10));	// ghi vao thanh phan text cua dia chi bien tren man hinh
	myNex.writeStr("dl.minpm25.txt", String(pm25_min_u32, 10));	// ghi vao thanh phan text cua dia chi bien tren man hinh

}
