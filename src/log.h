/**
 * @file log.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <stdarg.h>
#include "file.h"
#include "config.h"
#include "DataStructure.h"
#include "DS3231Driver.h"


#ifdef __cplusplus
	extern "C"
{
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
    DS3231_getStringDateTime(realTime, buffer_string, dateTime_string);
    return dateTime_string;
}


/**
 * @brief       Ghi cac thong tin debug vao the nho
 * 
 * @param[in]  nameFileSaveLogMessage_string : ten file ghi du lieu ("debug.txt")
 * @param[in]  format_string     : dinh dang chuoi ki tu ghi vao the nho
 * @param[in]  _connectionStatus : cau truc trang thai ket noi
 * @param[in]  ...               : day cac doi do truyen vao (##__VA_ARGS__)
 * 
 * @return ERROR_CODE
 */
ERROR_CODE writeLogMessageToFile(const char *nameFileSaveLogMessage_string, const char *format_string, ...)
{
    // if (connectionStatus_st.sdCardStatus == status_et::CONNECTED)
    if (true)
    {
        File writeFile;
        writeFile = SD.open(nameFileSaveLogMessage_string, FILE_APPEND);    // mo the nho
        if (writeFile)                                                      // kiem tra tinh trang o the nho
        {
            static char loc_buf[64];
            char * temp = loc_buf;
            int len;
            va_list arg;
            va_list copy;
            va_start(arg, format_string);
            va_copy(copy, arg);
            len = vsnprintf(NULL, 0, format_string, copy);
            va_end(copy);
            if(len >= sizeof(loc_buf)){
                temp = (char*)malloc(len+1);
                if(temp == NULL) {
                    va_end(arg);
                    return 0;
                }
            }
            vsnprintf(temp, len+1, format_string, arg);

            writeFile.printf("%s", temp);     // ghi tin nhan ra the nho
            writeFile.close();                                  // dong the nho
            va_end(arg);
            if(len >= sizeof(loc_buf)){
                free(temp);
            }
            return ERROR_NONE;
        } else {
            return ERROR_SD_CARD_FILE_NOT_FOUND;        // tra ve ma loi
        }
    } else {
        return ERROR_SD_CARD_INIT_FAILED;
    }
}

}

#define VERSION "2.5.1"
#ifdef USING_RTC
#define __DATETIME__    getDateTime((char *)"hh:mm:ss/DD/MMM/YYYY ")
#define LONG_LOG_FORMAT(letter, format)  "[%s]["#letter"][Version:%u:%s:%u] %s(): " format "\r\n", VERSION, __DATETIME__, __FILE__, __LINE__, __FUNCTION__
#else
#define __DATETIME__    "--:--:--/--/--/--"
#define LONG_LOG_FORMAT(letter, format)  "[" #letter "][%s:%u] %s(): " format "\r\n",  __FILE__, __LINE__, __FUNCTION__
#endif
#define SHORT_LOG_FORMAT(letter, format) "["#letter"]" format "\r\n"

#ifdef DEBUG_SERIAL
#define LOG_PRINT_NOTIFICATION(message)                         {DEBUG_SERIAL_PORT.println(message);}

#ifdef USING_SDCARD
#define NAME_FILE_SAVE_LOG_MESSAGE "debug.txt"

#define LOG_PRINT_INFORMATION(format, ...)	   		            {DEBUG_SERIAL_PORT.printf(SHORT_LOG_FORMAT(INFO, format), ##__VA_ARGS__); 	\
                                                                  writeLogMessageToFile( NAME_FILE_SAVE_LOG_MESSAGE, SHORT_LOG_FORMAT(INFO, format), ##__VA_ARGS__);}
                                                                


#define LOG_PRINT_ERROR(format, ...)		   		            {DEBUG_SERIAL_PORT.printf(LONG_LOG_FORMAT(ERR, format), ##__VA_ARGS__);	\
													              writeLogMessageToFile( NAME_FILE_SAVE_LOG_MESSAGE, LONG_LOG_FORMAT(ERROR, format), ##__VA_ARGS__);}

#else
#define LOG_PRINT_INFORMATION(format, ...)	   		            {DEBUG_SERIAL_PORT.printf(SHORT_LOG_FORMAT(INFO, format), ##__VA_ARGS__);}

#define LOG_PRINT_ERROR(format, ...)		   		            {DEBUG_SERIAL_PORT.printf(LONG_LOG_FORMAT(ERR, format), ##__VA_ARGS__);}
#endif

#define LOG_PRINT_ASSERT_INFO(condition, message)	            { if(!(condition))	DEBUG_SERIAL_PORT.printf(SHORT_LOG_FORMAT(INFO, message))}

#define LOG_PRINT_ASSERT_INFO2(condition, correctMessage, incorrectMessage)	{   if(!(condition))	DEBUG_SERIAL_PORT.printf(SHORT_LOG_FORMAT(INFO, correctMessage))    \
                                                                                else                DEBUG_SERIAL_PORT.printf(SHORT_LOG_FORMAT(INFO, incorrectMessage)) }

#define LOG_PRINT_ASSERT_ERROR(condition, message)	            { if(!(condition))	DEBUG_SERIAL_PORT.printf(LOG_FORMAT(ERR, message))}
#else 
#define LOG_PRINT_NOTIFICATION(message)		    	do{} while(0)

#define LOG_PRINT_INFORMATION(format, ...) 			do{} while(0)

#define LOG_PRINT_ERROR(format, ...)				do{} while(0)

#define LOG_PRINT_ASSERT_INFOR(condition, message)  do{} while(0)

#define LOG_PRINT_ASSERT_INFOR(condition, message)  do{} while(0)
#endif



#endif

