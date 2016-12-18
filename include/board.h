#ifndef BOARD_H
#define BOARD_H

#include "Arduino.h"
#include "SPI.h"

extern "C" {
#include "wirelessLove.h"
#include "protoLighthouse.h"
#include "fifo.h"
#include "sensors.h"
#include "logging.h"
}

#define BITS_LAST(k,n)  ((k) & ((1 << (n))-1))                  // get n-last bits from number k 
#define RANGE(k, m, n)    BITS_LAST(((k) >> m), ((n)-(m)))        // get bit range [m-n] from number k 
#define FIFO_SIZE 128

typedef enum _SPI_PINS{
    TRDY = 6,
    SS_N = 7,  
}_SPI_PINS; 

typedef enum SWEEPTYPE{
    NONE = 0, 
    VERTICAL = 1,
    HORIZONTAL = 2,
}SWEEPTYPE; 

typedef enum _ES{
    ES_WIFI_FAIL_INIT_NO_SHIELD = 1,
    ES_WIFI_FAIL_INIT_CANNOT_CONNECT,
    ES_WIFI_FAIL_UDP_SOCKET,
    ES_PROTO_FAIL_INIT,
    ES_PROTO_FAIL_ENCODE,
    ES_PROTO_FAIL_DECODE,
    ES_PROTO_ERROR,
    ES_PROTO_SUCCESS,
    ES_WIFI_SUCCESS, 
    ES_WIFI_ERROR,
}ES_PROTO;

#endif
