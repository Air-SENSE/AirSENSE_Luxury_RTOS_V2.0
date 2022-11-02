#ifdef O3_SENSOR_MQ131
#include <MQ131.h>

#define MQ131_INIT_SUCCESS   0x60
#define MQ131_INIT_UNSUCCESS 0x61

/**
 * @brief	Khoi tao module MQ131
 *
 * @return  MQ131_INIT_SUCCESS   0x60 : khoi dong sensor MQ131 thannh cong
 * @return  MQ131_INIT_UNSUCCESS 0x61 : khoi dong sensor MQ131 khong thannh cong
 */
uint32_t O3_init()
{
// MQ131.begin(2,4, LOW_CONCENTRATION, 1000000);  
	MQ131.begin(PIN_MQ131_POWER, PIN_MQ131_SENSOR, MQ131Model::LOW_CONCENTRATION, MQ131_DEFAULT_RL);  	// khoi dong sensor  MQ131
	LOG_PRINT_NOTIFICATION("Calibration in progress...");
	MQ131.calibrate();		//Calibrate the basic values
#ifdef	DEBUG_SERIAL
	LOG_PRINT_NOTIFICATION("Calibration done!");
	LOG_PRINT_NOTIFICATION("R0 = "+String(MQ131.getR0())+" Ohms");
	LOG_PRINT_NOTIFICATION("Time to heat = "+String(MQ131.getTimeToRead())+" s");
#endif
	if(MQ131.getO3(MQ131Unit::PPB) != 0.0)	// kiem tra co gia tri Rs duoc doc(read) khong
		return MQ131_INIT_SUCCESS;			// khoi dong sensor MQ131 thannh cong
	else
		return MQ131_INIT_UNSUCCESS;		// khoi dong sensor MQ131 khong thannh cong
}



/**
 * @brief	Luu tru gia tri vao bien O3_dislay_ppm, O3_dislay_ug
 * 
 * @param   O3_dislay_ppb_u32	gia tri O3 tren man hinh don vi ppb
 * @param   O3_min_ppb_u32		gia tri O3 nho nhat don vi ppb
 * @param   O3_max_ppb_u32		gia tri O3 lon nhat don vi ppb
 * @param   O3_display_ppm		gia tri O3 tren man hinh don vi ppm
 * @param   O3_display_ug		gia tri O3 tren man hinh don vi ug
 * @param   O3_min_ug			gia tri O3 nho nhat don vi ug
 * @param   O3_min_ppm			gia tri O3 nho nhat don vi ppm
 * @param   O3_max_ug			gia tri O3 lon nhat don vi ug
 * @param   O3_max_ppm			gia tri O3 lon nhat don vi ppm
 * 
 * @return  O3_dislay_ppb_u32
 */
uint32_t O3_getData(uint32_t *O3_dislay_ppb_u32, uint32_t O3_min_ppb_u32, uint32_t O3_max_ppb_u32, 
				float *O3_display_ppm, float *O3_display_ug, float *O3_min_ug, float *O3_min_ppm, float *O3_max_ug, float *O3_max_ppm)
{
	*O3_dislay_ppb_u32 = random(30,46);

	// lay gia tri min  cua O3 theo don vi ppb
	O3_min_ppb_u32 = (O3_min_ppb_u32 > *O3_dislay_ppb_u32 )	? O3_min_ppb_u32 : TFT_o3_ppb_u32;
	// lay gia tri max  cua O3 theo don vi ppb
	O3_max_ppb_u32 = (O3_max_ppb_u32 < *O3_dislay_ppb_u32 ) ? O3_max_ppb_u32 : TFT_o3_ppb_u32;

	*O3_display_ppm = float(*O3_dislay_ppb_u32)/1000;
	*O3_display_ug  = float(*O3_dislay_ppb_u32)*1.96;
	*O3_min_ug  	= float(O3_min_ppb_u32)*1.96;
	*O3_min_ppm 	= float(O3_min_ppb_u32)/1000;
	*O3_max_ug  	= float(O3_max_ppb_u32)*1.96;
	*O3_max_ppm 	= float(O3_max_ppb_u32)/1000;
	return *O3_dislay_ppb_u32;		// tra ve gia tri cua O3_display_ppb_u32 de kiem tra trong ham main
}

#endif
