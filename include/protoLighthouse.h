#ifndef PROTOLIGHTHOUSE_H
#define PROTOLIGHTHOUSE_H

#include "board.h"
#include "pb.h"

typedef struct _PROTO_LOVE{
    int         (* const encode_send_Proto)(uint16_t);
    int         (* const decode_config_Proto)(const pb_byte_t *, size_t);
    int         (* const initProto)(void); 
    int         (* const addSensor_Data)(float, int, uint16_t, uint16_t ); 
    void        (* const incrementSensorEntry)(uint16_t); 
    void        (* const resetSensorEntry)(uint16_t); 
}PROTO_LOVE;

extern PROTO_LOVE const protoLove; 

#endif
