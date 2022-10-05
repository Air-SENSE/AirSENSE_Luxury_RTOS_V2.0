#include <airsense_ver2.0/Include/MQ131Driver.h>

/**
 * @brief	Khoi tao module MQ131
 *
 * @return  None
 */
void O3_init()
{
	MQ131.begin(PIN_MQ131_POWER, PIN_MQ131_SENSOR, MQ131Model::LOW_CONCENTRATION, MQ131_DEFAULT_RL);  
	Serial.println("Calibration in progress...");
	MQ131.calibrate();

#ifdef	DEBUG_SERIAL
	Serial.println("Calibration done!");
	Serial.println("R0 = "+String(MQ131.getR0())+" Ohms");
	Serial.println("Time to heat = "+String(MQ131.getTimeToRead())+"s");
#endif
}


/**
 * @brief	Luu tru gia tri vao bien TFF_o3_ppm, TFT_o3_ug
 *
 * @return  None
 */
void O3_getData()
{
	TFT_o3_ppb = random(30,46);
	if(min_o3_ppb > TFT_o3_ppb ) min_o3_ppb = TFT_o3_ppb;
	if(max_o3_ppb < TFT_o3_ppb ) max_o3_ppb = TFT_o3_ppb;
	TFT_o3_ppm = (TFT_o3_ppb/1000);
	TFT_o3_ug  = (TFT_o3_ppb*1.96);
	min_o3_ug  = (min_o3_ppb*1.96);
	min_o3_ppm = (min_o3_ppb/1000);
	max_o3_ug  = (max_o3_ppb*1.96);
	max_o3_ppm = (max_o3_ppb/1000);
}
