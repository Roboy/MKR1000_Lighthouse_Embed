#ifndef WIRELESSLOVE_H
#define WIRELESSLOVE_H

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "protoLighthouse.h"

typedef enum ES_WIFI{
    ES_INIT_FAILED_NO_SHIELD = 1, 
    ES_INIT_FAILED_CANNOT_CONNECT, 
    ES_INIT_FAILED_UDP_SOCKET, 
    ES_ERROR,
    ES_SUCCESS, 
}ES_WIFI; 

typedef struct _WIFI_LOVE{
    int     (* const printMacAddress)(void);
    int     (* const printEncryptionType)(uint8_t); 
    int     (* const printAvailableNetworks)(void); 
    int     (* const printWifiStatus)(void); 
    int     (* const initWifi)(void); 
    int     (* const initUDPSockets)(void); 
    int     (* const sendUDPTestPacket)(void); 
    int     (* const sendUDPPacket)(const uint8_t *buffer, size_t size); 
    int     (* const receiveUDPPacket)(const uint8_t *buffer, size_t size); 
    int     (* const getConnectionStatus)(void);
}WIFI_LOVE;

extern WIFI_LOVE const  whylove;

#endif
