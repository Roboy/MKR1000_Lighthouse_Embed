#ifndef PROTOLIGHTHOUSE_H
#define PROTOLIGHTHOUSE_H

#include "board.h"
#include "pb.h"

typedef struct _PROTO_LOVE{
    int         (* const encode_send_Proto)(void);
    int         (* const decode_config_Proto)(const pb_byte_t *, size_t);
    int         (* const initProto)(void); 
    int         (* const addSensor_Data)(float, int); 
    void        (* const incrementSensorEntry)(void); 
    void        (* const resetSensorEntry)(void); 
}PROTO_LOVE;

extern PROTO_LOVE const protoLove; 

#endif
