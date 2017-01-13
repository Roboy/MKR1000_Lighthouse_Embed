#ifndef WIRELESSLOVE_H
#define WIRELESSLOVE_H

#include "board.h"

typedef struct _WIFI_LOVE{
    int     (* const printMacAddress)(void);
    int     (* const printEncryptionType)(uint8_t); 
    int     (* const printAvailableNetworks)(void); 
    int     (* const printWifiStatus)(void); 
    int     (* const initWifi)(void); 
    int     (* const initUDPSockets)(void); 
    int     (* const sendUDPTestPacket)(void); 
    int     (* const sendUDPPacket_TimeStamp)(const uint8_t *buffer, size_t size); 
    int     (* const sendUDPPacket)(const uint8_t *buffer, size_t size); 
    int     (* const receiveUDPPacket)(const uint8_t *buffer, size_t size); 
    int     (* const getConnectionStatus)(void);
}WIFI_LOVE;

extern WIFI_LOVE const  whylove;

#endif
