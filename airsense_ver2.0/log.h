#ifdef  _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include "DS3231Driver.h"
#include "config.h"
#include "RTClib.h"
#include "SD.h"

#ifdef __cplusplus
	extern "C"
{

#define VERSION "2.5.1"
#define __DATETIME__    getDateTime("hh:mm:ss/DD/MMM/YYYY ")
#define NAME_FILE_SAVE_LOG_MESSAGE "debug.txt"
#define SHORT_LOG_FORMAT(letter, format) "["#letter"]" format "\r\n"
#if USING_RTC
#define LOG_FORMAT(letter, format)  "[%s]["#letter"][Version:%u:%s:%u] %s(): " format "\r\n", VERSION, __DATETIME__, __FILE__, __LINE__, __FUNCTION__
#else 
#define LOG_FORMAT(letter, format)  "[" #letter "][%s:%u] %s(): " format "\r\n",  __FILE__, __LINE__, __FUNCTION__
#endif

#ifdef DEBUG_SERIAL
#define LOG_PRINT_NONE(message)						            {Serial.println(message);}

#define LOG_PRINT_INFORMATION(format, ...)	   		            {Serial.printf(SHORT_LOG_FORMAT(INFORMATION, format), ##__VA_ARGS__);	\
													             writeLogMessageToFile(NAME_FILE_SAVE_LOG_MESSAGE, SHORT_LOG_FORMAT(Notification, format), ##__VA_ARGS__);}

#define LOG_PRINT_ERROR(format, ...)		   		            {Serial.printf(LOG_FORMAT(ERROR, format), ##__VA_ARGS__);		 		\
													             writeLogMessageToFile(NAME_FILE_SAVE_LOG_MESSAGE, LOG_FORMAT(Notification, format), ##__VA_ARGS__);}

#define LOG_PRINT_ASSERT_INFOR(condition, message)	            { if(!(condition))	Serial.printf(SHORT_LOG_FORMAT(INFORMATION, message))}
#define LOG_PRINT_ASSERT_INFOR2(condition, message1, message2)	{ if(!(condition))	Serial.printf(SHORT_LOG_FORMAT(INFORMATION, message1))\
                                                                  else Serial.printf(SHORT_LOG_FORMAT(INFORMATION, message2))}

#define LOG_PRINT_ASSERT_ERROR(condition, message)	            { if(!(condition))	Serial.printf(LOG_FORMAT(ERROR, message))}
#else 
#define LOG_PRINT_NONE(message)						do{} while(0)

#define LOG_PRINT_INFORMATION(format, ...) 			do{} while(0)

#define LOG_PRINT_ERROR(format, ...)				do{} while(0)

#define LOG_PRINT_ASSERT_INFOR(condition, message)  do{} while(0)

#define LOG_PRINT_ASSERT_INFOR(condition, message)  do{} while(0)
#endif

#endif


/**
 * @brief lay chuoi ki tu thoi gian hien tai
 * @param buffer_string : chuoi dinh dang thoi gian
 * @input: char *buffer : chuoi dinh dang thoi gian
 * @output: const char *getDateTime(char * buffer_string)
 * @return const char* 
 */
char *getDateTime(char * buffer_string)
{
    return realTime.now().toString(buffer_string);       // tra ve chuoi ki tu thoi gian
}


/**
 * @brief Ghi cac thong tin debug vao the nho
 * @input:  const char *nameFileSaveLogMessage_string
 *          const char *format_string
 *          ...
 * @param nameFileSaveLogMessage_string : ten file ghi du lieu ("debug.txt")
 * @param format_string  : dinh dang chuoi ki tu ghi vao the nho
 * @param ...       : day cac doi do truyen vao (##__VA_ARGS__)
 * 
 * @output: int writeLogMessageToFile(const char *nameFileSaveLogMessage_string, const char *format_string, ...) : ma loi
 * @return int  : ma loi
 */
int writeLogMessageToFile(const char *nameFileSaveLogMessage_string, const char *format_string, ...)
{
#ifdef USING_SD_CARD
    File writeFile;
    writeFile = SD.open(nameFileSaveLogMessage_string, FILE_APPEND);    // mo the nho
    if (writeFile)                                                      // kiem tra tinh trang o the nho
    {
        writeFile.printf(format_string, ##__VA_ARGS__);     // ghi tin nhan ra the nho
        writeFile.close();                                  // dong the nho
        return ERROR_NONE;
    } else {
        return ERROR_FILE_NOT_FOUND;        // tra ve ma loi
    }
    
#else
    return ERROR_FILE_NOT_FOUND;            // tra ve ma loi
#endif
}

}
#endif
