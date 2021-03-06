#include "protoLighthouse.h"

#include "lighthouse_sensor.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#define MAX_NUMBER_SENSOR_VALUES 2
#define NUMBER_OF_SENSORS 8

bool                                        status; 
size_t                                      msg_len; 
static  uint32_t                            DataEntry[30] = {0}; 
static  uint32_t                            IndexVertical[30] = {0}; 
static  uint32_t                            IndexHorizontal[30] = {0}; 
mkr1000_lighthouse_sensor                   trackedSensor[NUMBER_OF_SENSORS]; 
mkr1000_lighthouse_configObject             configObjMsg; 

static int encode_send_Proto(uint16_t id);

static int initProto()
{
    uint8_t res = ES_PROTO_ERROR;

    for(int i = 0; i < NUMBER_OF_SENSORS; i++){
        trackedSensor[i] = mkr1000_lighthouse_sensor_init_zero;
    }
    configObjMsg = mkr1000_lighthouse_configObject_init_zero; 
    res = ES_PROTO_SUCCESS;
    enableLogging = false;
    return res; 
}    

static void resetSensorEntry(uint16_t id)
{
    LOG_d(logVERBOSE, "reset Senor entries of ID: ", id); 
    IndexHorizontal[id] = 0; 
    IndexVertical[id] = 0; 
    DataEntry[id] = 0; 
}

static void incrementSensorEntry(uint16_t id)
{
    uint32_t * d_entry = NULL; 
    d_entry = &DataEntry[id]; 

    (*d_entry)++;
    LOG_d(logVERBOSE, "incrementSensor Entry", *d_entry);  
    if((*d_entry) == MAX_NUMBER_SENSOR_VALUES)
    {
        LOG_d(logVERBOSE, "reached limit Sensor Entries ID: ", id); 
        encode_send_Proto(id); 
        resetSensorEntry(id); 
    }
}

static int addSensor_Data(float angle, int sweeptype, uint16_t id, uint16_t lighthouse)
{
    uint8_t res = ES_PROTO_ERROR;
    uint32_t * index_h = NULL; 
    uint32_t * index_v = NULL;
    uint32_t * d_entry = NULL; 

    LOG_d(logVERBOSE, "Add Sensor Data called. ID: ", id); 

    index_h = (uint32_t * ) &IndexHorizontal[id];
    index_v = (uint32_t * ) &IndexVertical[id];
    d_entry =  &DataEntry[id]; 

    if(sweeptype == HORIZONTAL)
    {
        trackedSensor[id].angles_h[(*index_h)] = angle;
        trackedSensor[id].angles_h_count++;

        trackedSensor[id].lighthouse_h[(*index_h)] = lighthouse;
        trackedSensor[id].lighthouse_h_count++;

        LOG_d(logINFO, "HORIZONTAL sweep added at index: ", (*index_h)); 
        LOG_d(logINFO, "Lighouse: ", lighthouse); 
        (*index_h)++; 
    }

    if(sweeptype == VERTICAL)
    {
        trackedSensor[id].angles_v[(*index_v)] = angle;
        trackedSensor[id].angles_v_count++;

        trackedSensor[id].lighthouse_v[(*index_v)] = lighthouse;
        trackedSensor[id].lighthouse_v_count++;

        LOG_d(logINFO, "VERTICAL sweep added at index: ", (*index_v)); 
        LOG_d(logINFO, "Lighouse: ", lighthouse); 
        (*index_v)++;
    }

    incrementSensorEntry(id); 

    res = ES_PROTO_SUCCESS; 
    return res; 
}

static int decode_trackedObj_Proto(size_t rcvd_msg_len, pb_byte_t * buffer)
{
    int res = ES_PROTO_ERROR; 


    LOG(logVERBOSE, "decoded tracked Object Message Proto"); 
    mkr1000_lighthouse_sensor rcvd_trackedSensor; 

    rcvd_trackedSensor = mkr1000_lighthouse_sensor_init_zero; 
    pb_istream_t stream = pb_istream_from_buffer(buffer, rcvd_msg_len); 
    status = pb_decode(&stream, mkr1000_lighthouse_sensor_fields, &rcvd_trackedSensor);  

    if(status){
        LOG(logINFO, "******************************************"); 
        LOG(logINFO, "decoded tracked Object Message Proto: "); 
        LOG_d(logINFO, "Vertical angels count: ", rcvd_trackedSensor.angles_v_count); 
        LOG_d(logINFO, "Horizontal angels count: ", rcvd_trackedSensor.angles_h_count); 
        LOG_d(logINFO, "Sensor ID: ", rcvd_trackedSensor.id); 
        res = ES_PROTO_SUCCESS; 
        return res; 
    }else{
        LOG(logINFO, "decoding failed"); 
        return res; 
    }
}

static int decode_config_Proto(const pb_byte_t * buffer, size_t rcvd_msg_len)
{
    uint8_t res = ES_PROTO_ERROR; 
    LOG(logVERBOSE_3, "decoded config Message Proto"); 
    mkr1000_lighthouse_configObject rcvd_configObjMsg = mkr1000_lighthouse_configObject_init_zero; 

    pb_istream_t stream = pb_istream_from_buffer(buffer, rcvd_msg_len); 

    status = pb_decode(&stream, mkr1000_lighthouse_trackedObjectConfig_fields, &rcvd_configObjMsg);  

    if(status){
        LOG(logVERBOSE, "decoded Config Message Proto: "); 
        LOG_d(logVERBOSE, "received IP: ", rcvd_configObjMsg.ip); 
        LOG_d(logVERBOSE, "received Port: ", rcvd_configObjMsg.port); 
    }else{
        LOG(logDEBUG, "decoding failed"); 
    }
    return res; 
}

static int encode_send_Proto(uint16_t id)
{
    uint8_t res = ES_PROTO_ERROR; 
    mkr1000_lighthouse_sensor trackedSensor_send = mkr1000_lighthouse_sensor_init_zero; 
    LOG_d(logINFO, "Encode and Send Protobuffer from Sensor ID: ", id); 

    if(trackedSensor[id].angles_h_count == 0 && trackedSensor[id].angles_v_count == 0)
    {
        LOG_d(logVERBOSE, "Horizontal and Vertical angle count empty. Leave", id); 
        return res; 
    }

    trackedSensor[id].id = id + 1; 
    trackedSensor_send = trackedSensor[id]; 

    pb_byte_t buffer[512] = {0};
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer)); 
   
    status = pb_encode(&stream, mkr1000_lighthouse_sensor_fields, &trackedSensor_send); 
    msg_len = stream.bytes_written; 
 
    if(!status)
    {
        LOG(logERROR, "Encoding failed!"); 
        LOG_d(logERROR, "angles h sensor 1: ", trackedSensor[id].angles_h_count); 
        LOG_d(logERROR, "angles v sensor 1: ", trackedSensor[id].angles_v_count); 
        Serial.println(PB_GET_ERROR(&stream)); 
    }

    res = whylove.sendUDPPacket(buffer, msg_len); 

    if(res != ES_WIFI_ERROR)
    {
        LOG(logVERBOSE, "Sended Protobuffer successfully via the UDP Socket"); 
        res = ES_PROTO_SUCCESS; 
    }

    decode_trackedObj_Proto(msg_len, buffer); 

    // reset protobuffer entries
    trackedSensor[id].angles_h_count = 0;
    trackedSensor[id].angles_v_count = 0;
    trackedSensor[id].timestamp_count = 0; 
    trackedSensor[id].lighthouse_h_count = 0; 
    trackedSensor[id].lighthouse_v_count = 0; 
    for(int i = 0; i < 20; ++i){
        trackedSensor[id].angles_h[i]           = 0;
        trackedSensor[id].angles_v[i]           = 0;
        trackedSensor[id].lighthouse_v[i]         = 0;
        trackedSensor[id].lighthouse_h[i]         = 0;
    }
    return res; 
}

PROTO_LOVE const protoLove = { encode_send_Proto, decode_config_Proto, initProto, addSensor_Data, incrementSensorEntry, resetSensorEntry}; 
