#pragma once
#ifndef _MQ131DRIVER_H_
#define _MQ131DRIVER_H_

#ifdef O3_SENSOR_MQ131

#include <MQ131.h>
#include "config.h"

void O3_init();
void O3_getData();

#endif
#endif