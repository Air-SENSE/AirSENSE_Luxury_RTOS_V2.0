#ifdef O3_SENSOR_MQ131
#include <MQ131.h>

/**
 * @brief	Khoi tao module MQ131
 *
 * @return  None
 */
uint32_t O3_init()
{
// MQ131.begin(2,4, LOW_CONCENTRATION, 1000000);  
	MQ131.begin(PIN_MQ131_POWER, PIN_MQ131_SENSOR, MQ131Model::LOW_CONCENTRATION, MQ131_DEFAULT_RL);  	// khoi dong sensor  MQ131
	LOG_PRINT_ERROR("Calibration in progress...");
	MQ131.calibrate();		//Calibrate the basic values
#ifdef	DEBUG_SERIAL
	LOG_PRINT_ERROR("Calibration done!");
	LOG_PRINT_ERROR("R0 = "+String(MQ131.getR0())+" Ohms");
	LOG_PRINT_ERROR("Time to heat = "+String(MQ131.getTimeToRead())+" s");
#endif
}



/**
 * @brief	Luu tru gia tri vao bien TFF_o3_ppm, TFT_o3_ug
 *
 * @return  None
 */
uint32_t O3_getData()
{
	TFT_o3_ppb_u32 = random(30,46);

	// lay gia tri min  cua O3 theo don vi ppb
	min_o3_ppb_u32 = (min_o3_ppb_u32 > TFT_o3_ppb_u32 )	? min_o3_ppb_u32 : TFT_o3_ppb_u32;
	// lay gia tri max  cua O3 theo don vi ppb
	max_o3_ppb_u32 = (max_o3_ppb_u32 < TFT_o3_ppb_u32 ) ? max_o3_ppb_u32 : TFT_o3_ppb_u32;

	TFT_o3_ppm = float(TFT_o3_ppb_u32)/1000;
	TFT_o3_ug  = float(TFT_o3_ppb_u32)*1.96;
	min_o3_ug  = float(min_o3_ppb_u32)*1.96;
	min_o3_ppm = float(min_o3_ppb_u32)/1000;
	max_o3_ug  = float(max_o3_ppb_u32)*1.96;
	max_o3_ppm = float(max_o3_ppb_u32)/1000;
}

#endif
