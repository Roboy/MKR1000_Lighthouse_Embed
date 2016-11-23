#include "protoLighthouse.h"

#include "lighthouse_sensor.pb.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

bool                                 status; 
size_t                                      msg_len; 
static uint32_t                             DataEntrySensor; 
mkr1000_lighthouse_trackedObject            trackedObjMsg; 
mkr1000_lighthouse_trackedObject_Sensor     trackedSensor; 
mkr1000_lighthouse_configObject             configObjMsg; 

static int encode_send_Proto()
{
    uint8_t res = ES_PROTO_ERROR; 
    trackedObjMsg.s_count = 3; 
    pb_byte_t buffer[512] = {0};

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
   
    status = pb_encode(&stream, mkr1000_lighthouse_trackedObject_fields, &trackedObjMsg); 
    msg_len = stream.bytes_written; 
 
    if(!status)
    {
        Serial.print("PROTOLIGHTHOUSE: Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    Serial.println("PROTOLIGHTHOUSE: Encoded protobuffer "); 
    Serial.print("PROTOLIGHTHOUSE: Message len: "); 
    Serial.println(msg_len); 
    res = whylove.sendUDPPacket(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        res = ES_PROTO_SUCCESS; 
        Serial.println("PROTOLIGHTHOUSE: Sended protobuffer successfull"); 
    }

    // reset protobuffer entries
    for(int i = 0; i < 3; ++i)
    {
        trackedObjMsg.s[i].has_id                      = false; 
        trackedObjMsg.s[i].timestamp_count             = 0; 
        trackedObjMsg.s[i].sensors0_count              = 0;  
        trackedObjMsg.s[i].angles0_h_count             = 0;
        trackedObjMsg.s[i].angles0_v_count             = 0;
        trackedObjMsg.s[i].sensors1_count              = 0;
        trackedObjMsg.s[i].angles1_h_count             = 0;
        trackedObjMsg.s[i].angles1_v_count             = 0;
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

static int addSensor_Data(const  void * const dataPointer)
{
    mkr1000_lighthouse_trackedObject_Sensor * sensordata = (mkr1000_lighthouse_trackedObject_Sensor*) dataPointer;  
    uint8_t res = ES_PROTO_ERROR;
    uint32_t id = (sensordata->id)-1;  

    trackedObjMsg.s[id].id                         = id; 
    trackedObjMsg.s[id].has_id                     = true; 

    trackedObjMsg.s[id].timestamp[DataEntrySensor] = sensordata->timestamp[0]; 
    trackedObjMsg.s[id].timestamp_count++; 

    trackedObjMsg.s[id].sensors0[DataEntrySensor]  = sensordata->sensors0[0]; 
    trackedObjMsg.s[id].sensors0_count++;  

    trackedObjMsg.s[id].angles0_h[DataEntrySensor] = sensordata->angles0_h[0]; 
    trackedObjMsg.s[id].angles0_h_count++;

    trackedObjMsg.s[id].angles0_v[DataEntrySensor] = sensordata->angles0_v[0]; 
    trackedObjMsg.s[id].angles0_v_count++;

    trackedObjMsg.s[id].sensors1[DataEntrySensor]  = sensordata->sensors1[0]; 
    trackedObjMsg.s[id].sensors1_count++;

    trackedObjMsg.s[id].angles1_h[DataEntrySensor] = sensordata->angles1_h[0]; 
    trackedObjMsg.s[id].angles1_h_count++;

    trackedObjMsg.s[id].angles1_v[DataEntrySensor] = sensordata->angles1_v[0]; 
    trackedObjMsg.s[id].angles1_v_count++;

    res = ES_PROTO_SUCCESS; 
    return res; 
}

static int incrementSensorEntry()
{
    uint8_t res = ES_PROTO_ERROR; 
    if(DataEntrySensor == 10){
        Serial.println("Max number of Sensor Data Entries received!");
    }else{
        DataEntrySensor++;
        res = ES_PROTO_SUCCESS; 
    }
    Serial.print("PROTOLIGHTHOUSE: incremented Sensor Entry to "); 
    Serial.println(DataEntrySensor); 
    return res; 
}

static void resetSensorEntry()
{
    Serial.println("PROTOLIGHTHOUSE: reset Sensor entry "); 
    DataEntrySensor = 0; 
}

static uint32_t getSenstorEntry()
{
    return DataEntrySensor; 
}

PROTO_LOVE const protoLove = { encode_send_Proto, decode_config_Proto, initProto, addSensor_Data, incrementSensorEntry, resetSensorEntry}; 
