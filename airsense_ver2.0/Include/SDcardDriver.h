#pragma once
#ifndef _SDCARDDRIVER_H_
#define _SDCARDDRIVER_H_

#ifdef USING_SD_CARD

#include <SD.h>
#include "NextionDriver.h"
#include "config.h"

File myFile;
File myFile2;
File myFile3;
File myFile4;

char char_array[30];
char fileNameCalib[] = "calib.txt";

char nameFileCalib1[16];
char nameFileCalib[16];

void SDcard_init();
void Screen_splitStringData();
void SDcard_readFile();
void SDcard_saveDataFile(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void runProgramWithSD();


#endif
#endif
