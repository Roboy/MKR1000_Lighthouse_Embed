#include "protoLighthouse.h"

#include "lighthouse_sensor.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

bool                                        status; 
size_t                                      msg_len; 
static uint32_t                             DataEntry; 
static uint32_t                             IndexVertical; 
static uint32_t                             IndexHorizontal; 

mkr1000_lighthouse_trackedObject            trackedObjMsg; 
mkr1000_lighthouse_trackedObject_Sensor     trackedSensor; 
mkr1000_lighthouse_configObject             configObjMsg; 

static int encode_send_Proto();


static int initProto()
{
    uint8_t res = ES_PROTO_ERROR;
    // currently contains 3 sensors
    trackedObjMsg = mkr1000_lighthouse_trackedObject_init_zero; 
    configObjMsg = mkr1000_lighthouse_configObject_init_zero; 
    res = ES_PROTO_SUCCESS;
    return res; 
}    

static void resetSensorEntry()
{
    LOG(logVERBOSE_3, "reset Sensor Entries"); 
    IndexVertical = 0; 
    IndexHorizontal = 0; 
    DataEntry = 0; 
}

static void incrementSensorEntry( )
{
    DataEntry++;
    LOG_d(logDEBUG, "incrementSensor Entry", DataEntry);  
    if(DataEntry == 4)
    {
        LOG(logDEBUG, "reached limit Sensor Entries "); 
        encode_send_Proto(); 
        resetSensorEntry(); 
    }
}

static int addSensor_Data(float angle, int sweeptype)
{
    LOG(logDEBUG, "Add Sensor Data called"); 
    uint8_t res = ES_PROTO_ERROR, id = 0;
    trackedObjMsg.s_count = 1; 
    if(sweeptype == HORIZONTAL)
    {
        trackedObjMsg.s[id].angles_h[IndexHorizontal] = angle;
        trackedObjMsg.s[id].angles_h_count++;

        LOG_d(logDEBUG, "HORIZONTAL sweep added at index: ", IndexHorizontal); 
        IndexHorizontal++; 
    }

    if(sweeptype == VERTICAL)
    {
        trackedObjMsg.s[id].angles_v[IndexVertical] = angle;
        trackedObjMsg.s[id].angles_v_count++;

        LOG_d(logDEBUG, "VERTICAL sweep added at index: ", IndexVertical); 
        IndexVertical++;
    }
    incrementSensorEntry(); 
    res = ES_PROTO_SUCCESS; 
    return res; 
}


static int decode_trackedObj_Proto(size_t rcvd_msg_len, pb_byte_t * buffer)
{
    LOG(logVERBOSE_3, "decoded tracked Object Message Proto"); 
    mkr1000_lighthouse_trackedObject        rcvd_trackedObjMsg; 

    rcvd_trackedObjMsg = mkr1000_lighthouse_trackedObject_init_zero; 

    pb_istream_t stream = pb_istream_from_buffer(buffer, rcvd_msg_len); 

    status = pb_decode(&stream, mkr1000_lighthouse_trackedObject_fields, &rcvd_trackedObjMsg);  

    if(status){
        LOG(logDEBUG, "decoded tracked Object Message Proto: "); 
        LOG_d(logDEBUG, "Angles_v_count: ", rcvd_trackedObjMsg.s[0].angles_v_count); 
        LOG_d(logDEBUG, "Angles_h_count: ", rcvd_trackedObjMsg.s[0].angles_h_count); 
    }else{
        LOG(logDEBUG, "decoding failed"); 
    }
}

static int decode_config_Proto(const pb_byte_t * buffer, size_t rcvd_msg_len)
{
    uint8_t res = ES_PROTO_ERROR; 
    LOG(logVERBOSE_3, "decoded config Message Proto"); 
    mkr1000_lighthouse_configObject rcvd_configObjMsg = mkr1000_lighthouse_configObject_init_zero; 

    pb_istream_t stream = pb_istream_from_buffer(buffer, rcvd_msg_len); 

    status = pb_decode(&stream, mkr1000_lighthouse_trackedObject_fields, &rcvd_configObjMsg);  

    if(status){
        LOG(logDEBUG, "decoded Config Message Proto: "); 
        LOG_d(logDEBUG, "received IP: ", rcvd_configObjMsg.id); 
        LOG_d(logDEBUG, "received Port: ", rcvd_configObjMsg.port); 
    }else{
        LOG(logDEBUG, "decoding failed"); 
    }
    return res; 
}


static int encode_send_Proto()
{
    uint8_t res = ES_PROTO_ERROR; 

    if(trackedObjMsg.s[0].angles_h_count == 0 && trackedObjMsg.s[0].angles_v_count == 0)
    {
        LOG(logVERBOSE_3, "Horizontal and Vertical angle count empty. Leave"); 
        return res; 
    }

    LOG(logVERBOSE, "#1 Encode proto #2 Send proto"); 
    pb_byte_t buffer[512] = {0};
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
   
    status = pb_encode(&stream, mkr1000_lighthouse_trackedObject_fields, &trackedObjMsg); 
    msg_len = stream.bytes_written; 
 
    if(!status)
    {
        LOG(logVERBOSE, "Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.sendUDPPacket(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        LOG(logDEBUG, "Sended Protobuffer successfully via the UDP Socket"); 
        res = ES_PROTO_SUCCESS; 
    }

    decode_trackedObj_Proto(msg_len, buffer); 

    // reset protobuffer entries
    for(int i = 0; i < 20; ++i)
    {
        trackedObjMsg.s[0].angles_h[i]             = 0;
        trackedObjMsg.s[0].angles_v[i]             = 0;
    }
    for(int i = 0; i < 3; ++i)
    {
        trackedObjMsg.s[i].angles_h_count = 0;
        trackedObjMsg.s[i].angles_v_count = 0;
    }
    return res; 
}

PROTO_LOVE const protoLove = { encode_send_Proto, decode_config_Proto, initProto, addSensor_Data, incrementSensorEntry, resetSensorEntry}; 
