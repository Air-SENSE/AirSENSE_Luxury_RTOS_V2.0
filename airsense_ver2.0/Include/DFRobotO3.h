#pragma once
#ifndef _DFROBOT03_H_
#define _DFROBOT03_H_

#ifdef O3_SENSOR_DF_ROBOT

#include <config.h>

#define COLLECT_NUMBER   20              // collect number, the collection range is 1-100
#define Ozone_IICAddress ADDRESS_3
/*   iic slave Address, The default is ADDRESS_3
       ADDRESS_0               0x70      // iic device address
       ADDRESS_1               0x71
       ADDRESS_2               0x72
       ADDRESS_3               0x73
*/

DFRobot_OzoneSensor Ozone;
uint32_t lastgetO3data = millis();

void O3_init();
void O3_getData();


#endif  // O3_SENSOR_DF_ROBOT

#endif // _DFROBOT03_H_