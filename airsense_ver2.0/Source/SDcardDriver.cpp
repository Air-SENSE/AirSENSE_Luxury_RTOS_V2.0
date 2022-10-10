#include <airsense_ver2.0/Include/SDcardDriver.h>

/**
 * @brief	ham de khoi tao the nho
 *
 * @return  None
 */ 
void SDcard_init()
{
    SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SDCARD);
    pinMode(SS, OUTPUT);

    if(!SD.begin(PIN_CS_SDCARD))
    { 
        TFT_SDcard = 0; 
        Serial.println("SD init false.");
    } else
    {
        TFT_SDcard = 1; 
        Serial.println("SD init done.");
    }
}




/**
 * @brief	split gia tri du lieu tu the nho
 *
 * @return  None
 */
void Screen_splitStringData()
{
#ifdef DEBUG_SERIAL
	Serial.print("Char_arr in split: "); 
	Serial.println(char_array);
#endif
char temp[30]   = "";
char humi[30]   = "";
char pm1[30]    = "";
char pm10[30]   = "";
char pm25[30]   = "";
char tempFl[30] = "";
char humiFl[30] = "";
	
	// split string
	char* pt = NULL;
	pt = strtok(char_array, "|");
	if(pt)
	strcpy(temp, pt);
	tempCalibInt = atoi(temp);
	
	pt = strtok(NULL, "|");
	if(pt)
	strcpy(humi, pt);
	humiCalibInt = atoi(humi);
	
	pt = strtok(NULL, "|");
	if(pt)
	strcpy(pm1, pt);
	pm1CalibInt = atoi(pm1);

	pt = strtok(NULL, "|");
	if(pt)
	strcpy(pm10, pt);
	pm10CalibInt = atoi(pm10);

	pt = strtok(NULL, "|");
	if(pt)
	strcpy(pm25, pt);
	pm25CalibInt = atoi(pm25);


		pt = strtok(NULL, "|");
	if(pt)
	strcpy(tempFl, pt);
	tempCalibFloat = atoi(tempFl);
	
		pt = strtok(NULL, "|");
	if(pt)
	strcpy(humiFl, pt);
	humiCalibFloat = atoi(humiFl);
#ifdef DEBUG_SERIAL
	Serial.println("After Split: ");
	Serial.println(tempCalibInt);
	Serial.println(humiCalibInt);
	Serial.println(pm1CalibInt);
	Serial.println(pm10CalibInt);
	Serial.println(pm25CalibInt);
	Serial.println(tempCalibFloat);
	Serial.println(humiCalibFloat);
	Serial.println("*********");
#endif
}


/**
 * @brief	Doc file tu trong the nho
 *
 * @return  None
 */
void SDcard_readFile() 
{
myFile3 = SD.open(nameFileCalib, FILE_READ);
String finalString = "";

while (myFile3.available())
{
	finalString += (char)myFile3.read();
}
	strcpy(char_array, finalString.c_str());
#ifdef DEBUG_SERIAL
	Serial.println("--- Reading file ---");
	Serial.print("Char_array: ");
	Serial.println(char_array);
#endif
	Screen_SplitStringData();
	myFile3.close();
}

/**
 * @brief	ham de luu gia tri vao the nho
 * @param	hum - gia tri do am
 * @param	tem - gia tri nhiet do
 * @param	pm1 - pm1.0
 * @param	pm25 - pm2.5
 * @param	pm10 - pm10
 * @param	O3ppb - 03
 * @param	O3ppm - 03
 * @param	O3ug - 03
 * @param	pm25_min - muc min cua pm25
 * @param	pm25_max - muc max cua pm25
 * @return  None
 */
void SDcard_saveDataFile(uint32_t hum, uint32_t tem, uint32_t pm1, uint32_t pm25, uint32_t pm10, uint32_t O3ppb, uint32_t O3ppm, uint32_t O3ug , uint32_t pm25_min, uint32_t pm25_max)
{
	DateTime now = rtc.now();
	int getyear = now.year();
	uint8_t getmonth = now.month();
	uint8_t getday = now.day();
	uint8_t gethour = now.hour();
	uint8_t getminute = now.minute();
	uint8_t getsecond = now.second();
	uint32_t epochTime= now.unixtime();
	uint8_t y = getyear%2000;
	
	char filename[16];
	if (getday < 10 && getmonth < 10) sprintf(filename, "/%d-0%d-0%d.txt", y, getmonth, getday);
	if (getday < 10 && getmonth >= 10) sprintf(filename, "/%d-%d-0%d.txt", y, getmonth, getday);
	if (getday >= 10 && getmonth < 10) sprintf(filename, "/%d-0%d-%d.txt", y, getmonth, getday);
	if (getday >= 10 && getmonth >= 10) sprintf(filename, "/%d-%d-%d.txt", y, getmonth, getday);
	
	if (0 <= y && y <= 22 && 0 < getmonth && getmonth <= 12 && 0 < getday && getday <= 31)
	{
		myFile = SD.open(filename, FILE_WRITE);
		if(myFile)
		{
			char mes[256] = {0};
			sprintf(mes, "%12s,%4d/%d/%d,%d:%d:%d,%d,%.1f,%.1f,%d,%d,%d,%d,%d,%d,%.3f,%.1f", nameDevice1, getyear, getmonth, getday, gethour, getminute, getsecond, epochTime, tem, hum, pm1, pm25, pm10, pm25_min, pm25_max, O3ppb, O3ppm, O3ug);
	#ifdef	DEBUG_SERIAL
			Serial.println(mes);
	#endif
			TFT_SDcard = 1;
			myFile.println(mes);        
			myFile.close();
		}
		else
		{ 

	#ifdef	DEBUG_SERIAL
			//Serial.println("reconnect SD");
	#endif

			if(!SD.begin(PIN_CS_SD_CARD, SPI)) TFT_SDcard = 0; 
			else TFT_SDcard = 1; 

		}
	} 
	else 
	{
#ifdef	DEBUG_SERIAL
	Serial.println("--------------Loi lay ngay sai: "+String(filename)+"---------------");
	//rtc.begin();
#endif
	}
}

/**
 * @brief	Chuong trinh hoat dong cua the nho
 *
 * @return  None
 */
void runProgramWithSD()
{
	// doc gia tri tu man hinh
	Screen_GetDisplayData();
	// neu man hinh moi khoi tao
	if( tempFromDisplay == 0 && humiFromDisplay == 0 && pm1FromDisplay == 0 && pm10FromDisplay == 0 && pm25FromDisplay == 0)
	{
		// doc gia tri tu the nho
		SDcard_ReadFile();
		// gui gia tri len man hinh calib
		Screen_DisplayCalibData();
	}
	else
	{
		// doc gia tri tu calib vao the sd
		Screen_SaveCalibData2SDcard();
		// doc nguoc lai the sd
		SDcard_ReadFile();
		// hien thi lai len man hinh
		Screen_DisplayCalibData();
	}
}
