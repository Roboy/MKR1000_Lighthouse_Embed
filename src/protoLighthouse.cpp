#include "protoLighthouse.h"

#include "lighthouse_sensor.pb.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

bool                                        status; 
size_t                                      msg_len; 
static uint32_t                             DataEntrySensor; 
mkr1000_lighthouse_trackedObject            trackedObjMsg; 
mkr1000_lighthouse_trackedObject_Sensor     trackedSensor; 
mkr1000_lighthouse_configObject             configObjMsg; 

static int encode_send_Proto()
{
    uint8_t res = ES_PROTO_ERROR; 

    if(trackedObjMsg.s[0].angles_h_count == 0 && trackedObjMsg.s[0].angles_v_count == 0)
    {
        return res; 
    }

    pb_byte_t buffer[512] = {0};
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
   
    status = pb_encode(&stream, mkr1000_lighthouse_trackedObject_fields, &trackedObjMsg); 
    msg_len = stream.bytes_written; 
 
    if(!status)
    {
        Serial.print("PROTOLIGHTHOUSE: Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.sendUDPPacket(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        res = ES_PROTO_SUCCESS; 
    }

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

static int decode_config_Proto()
{
    uint8_t res = ES_PROTO_ERROR; 
    configObjMsg = mkr1000_lighthouse_configObject_init_zero; 
    return res; 
}

static int initProto()
{
    uint8_t res = ES_PROTO_ERROR;
    // currently contains 3 sensors
    trackedObjMsg = mkr1000_lighthouse_trackedObject_init_zero; 
    configObjMsg = mkr1000_lighthouse_configObject_init_zero; 
    res = ES_PROTO_SUCCESS;
    return res; 
}    

static int addSensor_Data(float angle, int sweeptype)
{
    uint8_t res = ES_PROTO_ERROR, id = 0;
    trackedObjMsg.s_count = 1; 
    if(sweeptype == HORIZONTAL)
    {
        trackedObjMsg.s[id].angles_h[DataEntrySensor] = angle;
        trackedObjMsg.s[id].angles_h_count++;
    }

    if(sweeptype == VERTICAL)
    {
        trackedObjMsg.s[id].angles_v[DataEntrySensor] = angle;
        trackedObjMsg.s[id].angles_v_count++;
    }

    res = ES_PROTO_SUCCESS; 
    return res; 
}

static int incrementSensorEntry()
{
    uint8_t res = ES_PROTO_ERROR; 
    if(DataEntrySensor == 20){
        Serial.println("Max number of Sensor Data Entries received!");
    }else{
        DataEntrySensor++;
        res = ES_PROTO_SUCCESS; 
    }
    return res; 
}

static void resetSensorEntry()
{
    DataEntrySensor = 0; 
}

static uint32_t getSenstorEntry()
{
    return DataEntrySensor; 
}

PROTO_LOVE const protoLove = { encode_send_Proto, decode_config_Proto, initProto, addSensor_Data, incrementSensorEntry, resetSensorEntry}; 
