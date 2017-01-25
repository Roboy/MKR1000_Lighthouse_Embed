#ifndef WIRELESSLOVE_H
#define WIRELESSLOVE_H

#include "board.h"
#include <WiFi101.h>
#include <WiFiUdp.h>

typedef struct _WIFI_LOVE{
    uint16_t     (* const getCmndPort)(void); 
    uint32_t     (* const getLocalIP)(void); 

    int     (* const printWifiStatus)(void); 
    int     (* const initWifi)(void); 
    int     (* const initUDPSockets)(void); 
    int     (* const fmsgTest_s)(void); 
    int     (* const fmsgBroadcast_s)(const uint8_t * buffer, size_t size); 
    int     (* const fmsgLogging_s)(const uint8_t * buffer, size_t size); 
    int     (* const fmsgSensorDataT_s)(const uint8_t *buffer, size_t size); 
    int     (* const fmsgSensorData_s)(const uint8_t *buffer, size_t size); 
    bool    (* const fmsgConfig_r)(); 
    int     (* const getConnectionStatus)(void);
}WIFI_LOVE;

extern WIFI_LOVE const  whylove;

#endif
