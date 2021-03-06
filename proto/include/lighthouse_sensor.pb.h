/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.8-dev at Sat Dec 17 22:10:49 2016. */

#ifndef PB_MKR1000_LIGHTHOUSE_LIGHTHOUSE_SENSOR_PB_H_INCLUDED
#define PB_MKR1000_LIGHTHOUSE_LIGHTHOUSE_SENSOR_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _mkr1000_lighthouse_loggingObject {
    pb_callback_t message;
/* @@protoc_insertion_point(struct:mkr1000_lighthouse_loggingObject) */
} mkr1000_lighthouse_loggingObject;

typedef struct _mkr1000_lighthouse_commandObject {
    bool has_command;
    int32_t command;
/* @@protoc_insertion_point(struct:mkr1000_lighthouse_commandObject) */
} mkr1000_lighthouse_commandObject;

typedef struct _mkr1000_lighthouse_configObject {
    bool has_ip;
    int32_t ip;
    bool has_port;
    int32_t port;
/* @@protoc_insertion_point(struct:mkr1000_lighthouse_configObject) */
} mkr1000_lighthouse_configObject;

typedef struct _mkr1000_lighthouse_sensor {
    int32_t id;
    pb_size_t timestamp_count;
    int32_t timestamp[10];
    pb_size_t lighthouse_h_count;
    int32_t lighthouse_h[10];
    pb_size_t lighthouse_v_count;
    int32_t lighthouse_v[10];
    pb_size_t angles_h_count;
    float angles_h[10];
    pb_size_t angles_v_count;
    float angles_v[10];
/* @@protoc_insertion_point(struct:mkr1000_lighthouse_sensor) */
} mkr1000_lighthouse_sensor;

typedef struct _mkr1000_lighthouse_trackedObjectConfig {
    int32_t id;
    pb_callback_t ip;
    int32_t logging_port;
    int32_t sensordata_port;
    int32_t command_port;
/* @@protoc_insertion_point(struct:mkr1000_lighthouse_trackedObjectConfig) */
} mkr1000_lighthouse_trackedObjectConfig;

/* Default values for struct fields */

/* Initializer values for message structs */
#define mkr1000_lighthouse_configObject_init_default {false, 0, false, 0}
#define mkr1000_lighthouse_trackedObjectConfig_init_default {0, {{NULL}, NULL}, 0, 0, 0}
#define mkr1000_lighthouse_loggingObject_init_default {{{NULL}, NULL}}
#define mkr1000_lighthouse_commandObject_init_default {false, 0}
#define mkr1000_lighthouse_sensor_init_default   {0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define mkr1000_lighthouse_configObject_init_zero {false, 0, false, 0}
#define mkr1000_lighthouse_trackedObjectConfig_init_zero {0, {{NULL}, NULL}, 0, 0, 0}
#define mkr1000_lighthouse_loggingObject_init_zero {{{NULL}, NULL}}
#define mkr1000_lighthouse_commandObject_init_zero {false, 0}
#define mkr1000_lighthouse_sensor_init_zero      {0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}

/* Field tags (for use in manual encoding/decoding) */
#define mkr1000_lighthouse_loggingObject_message_tag 1
#define mkr1000_lighthouse_commandObject_command_tag 1
#define mkr1000_lighthouse_configObject_ip_tag   1
#define mkr1000_lighthouse_configObject_port_tag 2
#define mkr1000_lighthouse_sensor_id_tag         1
#define mkr1000_lighthouse_sensor_timestamp_tag  2
#define mkr1000_lighthouse_sensor_lighthouse_h_tag 3
#define mkr1000_lighthouse_sensor_lighthouse_v_tag 4
#define mkr1000_lighthouse_sensor_angles_h_tag   5
#define mkr1000_lighthouse_sensor_angles_v_tag   6
#define mkr1000_lighthouse_trackedObjectConfig_id_tag 1
#define mkr1000_lighthouse_trackedObjectConfig_ip_tag 2
#define mkr1000_lighthouse_trackedObjectConfig_logging_port_tag 3
#define mkr1000_lighthouse_trackedObjectConfig_sensordata_port_tag 4
#define mkr1000_lighthouse_trackedObjectConfig_command_port_tag 5

/* Struct field encoding specification for nanopb */
extern const pb_field_t mkr1000_lighthouse_configObject_fields[3];
extern const pb_field_t mkr1000_lighthouse_trackedObjectConfig_fields[6];
extern const pb_field_t mkr1000_lighthouse_loggingObject_fields[2];
extern const pb_field_t mkr1000_lighthouse_commandObject_fields[2];
extern const pb_field_t mkr1000_lighthouse_sensor_fields[7];

/* Maximum encoded size of messages (where known) */
#define mkr1000_lighthouse_configObject_size     22
/* mkr1000_lighthouse_trackedObjectConfig_size depends on runtime parameters */
/* mkr1000_lighthouse_loggingObject_size depends on runtime parameters */
#define mkr1000_lighthouse_commandObject_size    11
#define mkr1000_lighthouse_sensor_size           441

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define LIGHTHOUSE_SENSOR_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
