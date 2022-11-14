/**
 * @file WiFiManager.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __WIFIMANAGER_H__
#define __WIFIMANAGER_H__

#include <WiFi.h>
#include <string>
#include "config.h"
#include "log.h"

#define ERROR_WIFI_INIT_FAILED 0xa1

/**
 * @brief WiFi initialized
 * 
 * @return ERROR_CODE 
 */
ERROR_CODE WIFI_init()
{
#if (defined(SSID) && defined(PASSWORD))
    if (WiFi.begin(SSID, PASSWORD) == wl_status_t::WL_CONNECTED)
    {
        LOG_PRINT_INFORMATION("WiFi connected!");
        LOG_PRINT_INFORMATION("IP: %s", ((WiFi.localIP).toString()).c_str());
        return ERROR_NONE;
    } else {
        LOG_PRINT_INFORMATION("WiFi initialized failed!");
        return ERROR_WIFI_INIT_FAILED;
    }
#else
        if (WiFi.begin() == wl_status_t::WL_CONNECTED)
    {
        LOG_PRINT_INFORMATION("WiFi initialized successfully!");
        return ERROR_NONE;
    } else {
        LOG_PRINT_INFORMATION("WiFi initialized failed!");
        return ERROR_WIFI_INIT_FAILED;
    }
#endif
}


#endif
