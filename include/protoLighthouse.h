#ifndef PROTOLIGHTHOUSE_H
#define PROTOLIGHTHOUSE_H

#include "board.h"
#include "pb.h"

typedef struct _PROTO_LOVE{
    int         (* const initProto)(void); 
    int         (* const decode_config_Proto)(const pb_byte_t *, size_t);
    int         (* const encode_trackedObjConfig)(uint32_t, uint16_t);
    int         (* const encode_loggingObject)(const char *);
}PROTO_LOVE;

extern PROTO_LOVE const protoLove; 

#endif
