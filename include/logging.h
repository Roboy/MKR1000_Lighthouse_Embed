#ifndef LOGGING_H
#define LOGGING_H

#include "board.h"

static bool enableLogging = true; 

typedef enum TLogLevel 
{
    logERROR = 1,
    logWARNING,
    logDEBUG,
    logINFO,
    logVERBOSE,
    log_VERBOSE_1,
    log_VERBOSE_2,
    log_VERBOSE_3,
}TLogLevel; 

typedef enum FlushInterface{
    F_SERIAL  = 1, 
    F_WIFI    = 2, 
}FlushInterface;

typedef struct LogI
{
    void                    ( * const GetLogString)(TLogLevel, char *); 
    TLogLevel               ( * const GetReportingLevel)(void); 
    void                    ( * const setFlushInterface)(FlushInterface);
}LogI; 

extern LogI const logi; 

typedef void (* FlushI)(void); 

typedef struct LogString
{
    size_t length; 
    char * buff; 
    TLogLevel messageLevel;  
    FlushI flushI; 
}LogString; 

#define LOG(level, message) \
    if (level > (logi.GetReportingLevel()) || false == enableLogging); \
    else logi.GetLogString(level, message)

#endif
