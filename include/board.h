#ifndef BOARD_H
#define BOARD_H

#include "Arduino.h"
#include "wirelessLove.h"
#include "protoLighthouse.h"
#include "sensors.h"
#include "fifo.h"
#include "logging.h"

typedef enum _SENSORTYPE{
    SENSOR_1 = 1,
    SENSOR_2 = 2,
    SENSOR_3 = 3, 
}SENSORTYPE; 

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
