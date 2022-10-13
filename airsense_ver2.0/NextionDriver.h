/************************************************************
**    TFT_o3_ppb(int)  TFT_o3_ppm(float)  TFT_o3_ug(int)  **    
**    min_o3_ppb(int)  min_o3_ppm(float)  min_o3_ug(int)  **
**    max_o3_ppb(int)  max_o3_ppm(float)  max_o3_ug(int)  **
**    TFT_temp(uint8_t)            TFT_wifi               **
**    TFT_humi(uint8_t)            TFT_SDcard             **
**    TFT_temp_F(uint16_t)         Rx: pin 33             **
**    TFT_pm25(int)                Tx: pin 25             **
**    TFT_pm10(int)                                       **
**    TFT_time(string)                                    **
************************************************************/

/* Khai bao thu vien man hinh TFT */
#pragma once
#include <iostream>
#include <string>
#include "string.h"
#include <sstream>
#include "config.h"
#include "EasyNextionLibrary.h"
#include <EEPROM.h>

EasyNex myNex(Serial);


/**
 * @brief	Khoi tao man hinh
 *
 * @return  None
 */
void Screen_init(){
	myNex.begin(NEXTION_BAUDRATE);
	EEPROM.begin(EEPROM_SIZE);
}


/**
 * @brief Lay gia tri Calib tu man hinh
 * 
 * @param objectName 
 * 
 * @return uint16_t 
 */
uint16_t getCalibData(const char *objectName)
{
	uint16_t calibValue = myNex.readNumber(objectName);
	return (calibValue == ERROR_READ_DISPLAY) ? 0 : calibValue;
}


/**
 * @brief	Doc du lieu tu man hinh va du vao cac bien
 *
 * @return  None
 */
void Screen_getCalibData()
{
	temperature_calibInt_u16     =  getCalibData("calibEdit.n0.val");
	humidity_calibInt_u16        =  getCalibData("calibEdit.n1.val");
	pm1_calibInt_u32             =  getCalibData("calibEdit.n2.val");
	pm10_calibInt_u32            =  getCalibData("calibEdit.n3.val");
	pm25_calibInt_u32            =  getCalibData("calibEdit.n4.val");
	tempperature_calibFloat_u16  =  getCalibData("calibEdit.n5.val"); 
	humidity_calibFloat_u16      =  getCalibData("calibEdit.n6.val"); 

// In cac du lieu tu man hinh ra 
#ifdef DEBUG_SERIAL
	Serial.println(temperature_calibInt_u16    ); 
	Serial.println(humidity_calibInt_u16       );
	Serial.println(pm1_calibInt_u32            );
	Serial.println(pm10_calibInt_u32           );
	Serial.println(pm25_calibInt_u32           );
	Serial.println(tempperature_calibFloat_u16 );
	Serial.println(humidity_calibFloat_u16     );
	Serial.println("--------------");
#endif
}



/**
 * @brief	Luu tru cac gia tri da calib tu man hinh vao the nho
 *
 * @return  None
 */
void Screen_saveCalibDataToSDcard()
{
	if( dipslay_temperatureInt_u16 < -100 || dipslay_temperatureInt_u16 >1000 || display_humidityInt_u16 < -100 || display_humidityInt_u16 >1000 || display_pm1_u16 < -100 || display_pm1_u16 >1000 || display_pm10_u16 < -100  || display_pm10_u16 >1000 || display_pm25_u16 < -100 || display_pm25_u16 >1000)
	{
#ifdef DEBUG_SERIAL
		Serial.println("----- *** Don't write to SD card *** ----");
#endif
	}
	else
	{
		myFile = SD.open(fileNameCalib, FILE_WRITE);
		if(myFile)
		{
			char message[256] = {0};
			sprintf(message,"%d|%d|%d|%d|%d|%d|%d|", temperature_calibInt_u16,   
													 humidity_calibInt_u16,  
													 pm1_calibInt_u32,
													 pm10_calibInt_u32,
													 pm25_calibInt_u32,
													 tempperature_calibFloat_u16,
													 humidity_calibFloat_u16 );

#ifdef DEBUG_SERIAL
			Serial.print("Message:");
			Serial.println(message);
#endif
			TFT_SDStatus = SD_Status_et::SD_CONNECTED;
			myFile.println(message);        
			myFile.close();
		}
		else
		{ 
			Serial.println("Reconnect SD");
			SDcard_init();
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
	myNex.writeNum("dl.n4.val", temperature_calibInt_u16    );
	myNex.writeNum("dl.n5.val", humidity_calibInt_u16       );
	myNex.writeNum("dl.n6.val", pm1_calibInt_u32            );
	myNex.writeNum("dl.n7.val", pm10_calibInt_u32           );
	myNex.writeNum("dl.n8.val", pm25_calibInt_u32           );
	myNex.writeNum("dl.n7.val", tempperature_calibFloat_u16 );
	myNex.writeNum("dl.n8.val", humidity_calibFloat_u16     );
}


/**
 * @brief	ghi cac gia tri len man hinh
 *
 * @return  None
 */
void Screen_displayData()
{
	myNex.writeNum("dl.wifi.val",TFT_wifiStatus);
	myNex.writeNum("dl.sd.val",TFT_SDStatus);

	myNex.writeStr("dl.time.txt",TFT_time_string);

	sprintf(TFT_string,"%.1f",TFT_temperature_C);
	myNex.writeStr("dl.temc.txt",TFT_string);

	sprintf(TFT_string,"%.1f",TFT_humidity_percent);
	myNex.writeStr("dl.hum.txt",TFT_string);

	sprintf(TFT_string,"%.1f",TFT_temperature_F);
	myNex.writeStr("dl.temf.txt",TFT_string);

	myNex.writeNum("dl.nppb.val",TFT_o3_ppb);
	sprintf(TFT_string,"%d",TFT_o3_ppb);
	myNex.writeStr("dl.sppb.txt",TFT_string);

	sprintf(TFT_string,"%.1f",TFT_o3_ug);
	myNex.writeStr("dl.sug.txt",TFT_string);

	if(TFT_o3_ppb<10)	sprintf(TFT_string,"0.00%d",TFT_o3_ppb);
	if(10<=TFT_o3_ppb && TFT_o3_ppb<100)	sprintf(TFT_string,"0.0%d",TFT_o3_ppb);
	if(100<=TFT_o3_ppb && TFT_o3_ppb<1000)	sprintf(TFT_string,"0.%d",TFT_o3_ppb);
	if(1000 <= TFT_o3_ppb)	sprintf(TFT_string,"%d.%d",int(TFT_o3_ppb/1000),int(TFT_o3_ppb%1000));
	myNex.writeStr("dl.sppm.txt",TFT_string);

	sprintf(TFT_string,"%d",min_o3_ppb);
	myNex.writeStr("dl.sminppb.txt",TFT_string);

	sprintf(TFT_string,"%.1f",min_o3_ug);
	myNex.writeStr("dl.sminug.txt",TFT_string);

	if(min_o3_ppb<10)	sprintf(TFT_string,"0.00%d",min_o3_ppb);
	if(10<=min_o3_ppb && min_o3_ppb<100)	sprintf(TFT_string,"0.0%d",min_o3_ppb);
	if(100<=min_o3_ppb && min_o3_ppb<1000)	sprintf(TFT_string,"0.%d",min_o3_ppb);
	if(1000 <= min_o3_ppb)	sprintf(TFT_string,"%d.%d",int(min_o3_ppb/1000),int(min_o3_ppb%1000));
	myNex.writeStr("dl.sminppm.txt",TFT_string);

	sprintf(TFT_string,"%d",max_o3_ppb);
	myNex.writeStr("dl.smaxppb.txt",TFT_string);

	sprintf(TFT_string,"%.1f",max_o3_ug);
	myNex.writeStr("dl.smaxug.txt",TFT_string);

	if(max_o3_ppb<10)	sprintf(TFT_string,"0.00%d",max_o3_ppb);
	if(10<=max_o3_ppb && max_o3_ppb<100)	sprintf(TFT_string,"0.0%d",max_o3_ppb);
	if(100<=max_o3_ppb && max_o3_ppb<1000)	sprintf(TFT_string,"0.%d",max_o3_ppb);
	if(1000 <= max_o3_ppb)	sprintf(TFT_string,"%d.%d",int(max_o3_ppb/1000),int(max_o3_ppb%1000));
	myNex.writeStr("dl.smaxppm.txt",TFT_string);

	myNex.writeNum("dl.npm25.val",TFT_pm25_u32);
	myNex.writeNum("dl.npm1.val",TFT_pm1_u32);
	myNex.writeNum("dl.npm10.val",TFT_pm10_u32);
	sprintf(TFT_string,"%d",TFT_pm1_u32);
	myNex.writeStr("dl.spm1.txt",TFT_string);
	sprintf(TFT_string,"%d",TFT_pm25_u32);
	myNex.writeStr("dl.spm25.txt",TFT_string);
	sprintf(TFT_string,"%d",TFT_pm10_u32);
	myNex.writeStr("dl.spm10.txt",TFT_string);
	sprintf(TFT_string,"%d",pm25_max_u32);
	myNex.writeStr("dl.maxpm25.txt",TFT_string);
	sprintf(TFT_string,"%d",pm25_min_u32);
	myNex.writeStr("dl.minpm25.txt",TFT_string);

}
