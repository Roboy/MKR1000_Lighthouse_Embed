#include "protoLighthouse.h"
#include "pb_encode.h"
#include "pb_decode.h"

static size_t msg_len; 
static bool status; 
static uint8_t buffer[512];
static mkr1000_lighthouse_trackedObject     trackedObjMsg; 
static mkr1000_lighthouse_configObject      configObjMsg; 
static uint32_t DataEntrySensor = 0; 

static int encode_send_Proto()
{
    uint8_t res = ES_PROTO_ERROR; 
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 

    status  = pb_encode(&stream,mkr1000_lighthouse_trackedObject_fields, &trackedObjMsg); 
    msg_len = stream.bytes_written; 

    if(!status){
        Serial.print("Encoding failed!"); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.sendUDPPacket(buffer, msg_len); 
    if(res != ES_ERROR){
        res = ES_PROTO_SUCCESS; 
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
    SensorData * sensordata = (SensorData*) dataPointer;  
    uint8_t res = ES_ERROR;
    uint32_t id = sensordata->id;  
    trackedObjMsg.values[id-1].id                         = id; 
    trackedObjMsg.values[id-1].timestamp[DataEntrySensor] = sensordata->timestamp; 
    trackedObjMsg.values[id-1].sensors0[DataEntrySensor]  = sensordata->sensor0; 
    trackedObjMsg.values[id-1].angles0_h[DataEntrySensor] = sensordata->angel0_h; 
    trackedObjMsg.values[id-1].angles0_v[DataEntrySensor] = sensordata->angel0_v; 
    trackedObjMsg.values[id-1].sensors1[DataEntrySensor]  = sensordata->sensor1; 
    trackedObjMsg.values[id-1].angles1_h[DataEntrySensor] = sensordata->angel1_h; 
    trackedObjMsg.values[id-1].angles1_v[DataEntrySensor] = sensordata->angel1_v; 
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
