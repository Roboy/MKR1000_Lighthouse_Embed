#include "protoLighthouse.h"

#include "lighthouse_sensor.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

mkr1000_lighthouse_loggingObject            loggingObjMsg; 
mkr1000_lighthouse_commandObject            commandObjMsg; 
mkr1000_lighthouse_configObject             configObjMsg; 

static bool status = false; 

static void 
clearProtos()
{
    configObjMsg        = mkr1000_lighthouse_configObject_init_zero; 
    commandObjMsg       = mkr1000_lighthouse_commandObject_init_zero; 
    configObjMsg        = mkr1000_lighthouse_configObject_init_zero; 
}

static int 
initProto()
{
    uint8_t res = ES_PROTO_ERROR;
    clearProtos(); 
    res = ES_PROTO_SUCCESS;
    enableLogging = true;
    return res; 
}    

static int 
decode_config_Proto(const pb_byte_t * buffer, size_t rcvd_msg_len)
{
    uint8_t res = ES_PROTO_ERROR; 
    LOG(logINFO, "decode config Message Proto"); 

    pb_istream_t stream = pb_istream_from_buffer(buffer, rcvd_msg_len); 
    status = pb_decode(&stream, mkr1000_lighthouse_configObject_fields, &configObjMsg);  

    if(status){
        LOG(logINFO, "decoded Config Message Proto: "); 
        LOG_d(logINFO, "IP Host PC: ", configObjMsg.ip); 
        LOG_d(logINFO, "Logging Port Target PC :  ", configObjMsg.logging_port); 
        LOG_d(logINFO, "Sensor Port Target PC  :  ", configObjMsg.sensor_port); 
    }else{
        LOG(logDEBUG, "decoding failed"); 
    }
    return res; 
}

static int 
encode_trackedObjConfig(uint32_t ip, uint16_t cmndPort_l)
{
    mkr1000_lighthouse_trackedObjectConfig      trackedObjConfMsg; 
    uint8_t res = ES_PROTO_ERROR; 
    LOG(logVERBOSE, "Encode Tracked Object Config"); 
    LOG_d(logVERBOSE, "Local IP Address of the MKR: ", ip); 
    LOG_d(logVERBOSE, "Local Command Port from the MKR: ", cmndPort_l); 

    pb_byte_t buffer[512] = {0};
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
  

    trackedObjConfMsg.ip = ip;  
    trackedObjConfMsg.command_port = cmndPort_l; 
    status = pb_encode(&stream, mkr1000_lighthouse_trackedObjectConfig_fields, &trackedObjConfMsg); 
    size_t msg_len = stream.bytes_written; 
    LOG_d(logVERBOSE, "Message len encode protobuf: ", msg_len);  
 
    if(!status)
    {
        LOG(logERROR, "Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.fmsgBroadcast_s(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        LOG(logINFO, "Sended Protobuffer successfully via the UDP Socket"); 
        res = ES_PROTO_SUCCESS; 
    }else{
        LOG(logERROR, "Sending failed!"); 
        res = ES_PROTO_ERROR; 
    }
    clearProtos(); 
    return res; 
}

static int 
encode_loggingObject(const char * msg)
{
    uint8_t res = ES_PROTO_ERROR; 
    LOG(logINFO, "Encode Logging Object"); 

    pb_byte_t buffer[512] = {0};
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
  
    strncpy(loggingObjMsg.message, msg, sizeof loggingObjMsg.message);    
    status = pb_encode(&stream, mkr1000_lighthouse_loggingObject_fields, &loggingObjMsg); 
    size_t msg_len = stream.bytes_written; 

    if(!status)
    {
        LOG(logERROR, "Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.fmsgLogging_s(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        LOG(logVERBOSE, "Sended Protobuffer successfully via the UDP Socket"); 
        res = ES_PROTO_SUCCESS; 
    }

    clearProtos(); 
    return res; 
}

PROTO_LOVE const protoLove = { initProto, decode_config_Proto, encode_trackedObjConfig, encode_loggingObject }; 
