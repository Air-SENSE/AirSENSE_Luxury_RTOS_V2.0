#pragma once

#if(defined(PIN_TX_TFLP01) && defined(PIN_RX_TFLP01))

#include "config.h"

// uint32_t     TFLP01_pm10sum_u32		= 0;
// uint32_t     TFLP01_pm25sum_u32 	= 0;
// uint32_t     TFLP01_pm1sum_u32 		= 0;
// uint32_t	 dataTFLP01count_u32 	= 0;
bool 	     TFLP01_readStatus_bl  	= false;
uint32_t	 TFT_pm1_u32		   	= 0;
uint32_t	 TFT_pm25_u32		    = 0;
uint32_t	 TFT_pm10_u32			= 0;
uint32_t	 pm25_min_u32		   	= 1000;
uint32_t	 pm25_max_u32		   	= 0;


/**
 * @brief  Return CRC trong data response của TF-LP-01
 *
 * @return  None
 */
uint16_t crc16_modbus(uint8_t *modbusdata , uint16_t Length) 
{
	uint16_t crc16 = 0xFFFF;
	for (uint16_t i = 0; i < Length; i++)
	{
		crc16 ^= modbusdata[i];
		for (uint16_t j = 0; j < 8; j++)
		{
			if ((crc16 & 0x01) == 1) crc16 = (crc16 >> 1) ^ 0xA001; 
			else  crc16 >>= 1;
		}
	}
	return crc16;
}


/**
 * @brief Setup TF-LP-01
 *
 * @return  None
 */
void TFLP01_init()
{
	Serial2.begin(115200);		// khoi dong Serial2
}


/**
 * @brief Doc du lieu tu cam bien bui
 *
 * @return  None
 */
void TFLP01_getData()
{
	uint8_t TFLP01_data_a8[17] = {0};
	if (Serial2.available() > 0) 
	{  
		Serial.println("Check: ");
		for(uint8_t i=0; i<17; i++)
		{
			Serial.print(i);
			Serial.print(": ");
			byte TFLP01_readData_byte = Serial2.read();			// doc tung byte du lieu tu TFLP01 qua Serial2
			TFLP01_data_a8[i] = (uint8_t)TFLP01_readData_byte;	// luu du lieu vua doc duoc vao mang du lieu cua TFLP01
			Serial.println(TFLP01_readData_byte);
		}
		
		TFLP01_readStatus_bl = true;
	}

	// lay du lieu tam thoi (chua co datasheet)
	if(TFLP01_readStatus_bl == true)
	{
		TFLP01_readStatus_bl = false;
		TFT_pm1_u32  = TFLP01_data_a8[9]  + TFLP01_data_a8[10]*256 + pm1_calibInt_u32;
		TFT_pm25_u32 = TFLP01_data_a8[11] + TFLP01_data_a8[12]*256 + pm25_calibInt_u32;
		TFT_pm10_u32 = TFLP01_data_a8[13] + TFLP01_data_a8[14]*256 + pm10_calibInt_u32;

		if(TFT_pm25_u32 != 255)
		{
			if(pm25_max_u32 < TFT_pm25_u32) pm25_max_u32 = TFT_pm25_u32;
			if(pm25_min_u32 > TFT_pm25_u32) pm25_min_u32 = TFT_pm25_u32;
		}

#ifdef  DEBUG_SERIAL
		// in du lieu ra man hinh Serial
		for(uint8_t i=0; i < 17; i++)
		{
			Serial.print(i);
			Serial.print(": ");
			Serial.println(TFLP01_data_a8[i]);
		}
		Serial.println("Pm2.5: " + String(TFT_pm25_u32) + "    Pm10: " + String(TFT_pm10_u32));
#endif

	}  
}

#endif
