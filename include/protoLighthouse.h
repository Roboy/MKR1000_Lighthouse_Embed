#ifndef PROTOLIGHTHOUSE_H
#define PROTOLIGHTHOUSE_H
#include "lighthouse_sensor.pb.h"
#include "LightHouseTimer.h"
#include "WirelessLove.h"

typedef enum ES_PROTO{
    ES_PROTO_FAIL_INIT,
    ES_PROTO_FAIL_ENCODE,
    ES_PROTO_FAIL_DECODE,
    ES_PROTO_ERROR, 
    ES_PROTO_SUCCESS,
}ES_PROTO; 

typedef struct _PROTO_LOVE{
    int         (* const encode_send_Proto)(void);
    int         (* const decode_config_Proto)(void);
    int         (* const initProto)(void); 
    int         (* const addSensor_Data)(const void * const); 
    int         (* const incrementSensorEntry)(void); 
    void        (* const resetSensorEntry)(void); 
}PROTO_LOVE;

extern PROTO_LOVE const protoLove; 

#endif
