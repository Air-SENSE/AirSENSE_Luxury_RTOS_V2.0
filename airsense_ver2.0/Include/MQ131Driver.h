#pragma once
#ifndef _MQ131DRIVER_H_
#define _MQ131DRIVER_H_

#ifdef O3_SENSOR_MQ131

#include <MQ131.h>

void O3_Init();
void O3_GetData();

#endif	// _MQ131DRIVER_H_
#endif  // O3_SENSOR_MQ131