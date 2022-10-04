#include <DFRobotO3.h>

/**
 * @brief	Khoi tao module Ozone
 *
 * @return  None
 */
void O3_Init()
{
    if(!Ozone.begin(Ozone_IICAddress)) 
    {
#ifdef	DEBUG_SERIAL
        Serial.println("I2c device number error !");
#endif
    }  
#ifdef	DEBUG_SERIAL
        Serial.println("I2c connect success !");
#endif
    Ozone.SetModes(MEASURE_MODE_PASSIVE);

}


/**
 * @brief	Luu tru gia tri vao bien TFF_o3_ppm, TFT_o3_ug
 *
 * @return  None
 */
void O3_GetData()
{
    if((millis()-lastgetO3data>5000) || (millis()<lastgetO3data))
    {
        TFT_o3_ppb = Ozone.ReadOzoneData(COLLECT_NUMBER);
        if(min_o3_ppb > TFT_o3_ppb ) min_o3_ppb = TFT_o3_ppb;
        if(max_o3_ppb < TFT_o3_ppb ) max_o3_ppb = TFT_o3_ppb;
        TFT_o3_ppm = float(TFT_o3_ppb)/1000;
        TFT_o3_ug  = float(TFT_o3_ppb)*1.96;
        min_o3_ug  = float(min_o3_ppb)*1.96;
        min_o3_ppm = float(min_o3_ppb)/1000;
        max_o3_ug  = float(max_o3_ppb)*1.96;
        max_o3_ppm = float(max_o3_ppb)/1000;
        lastgetO3data = millis();
    }
}