#include "logging.h"

static LogString * logString; 
static TLogLevel currentLoglevel = logVERBOSE; 

static void printWiFiUDP()
{
    Serial.print("WiFi print:"); 
    Serial.println(logString->buff); 
}

static void printSerial()
{
    Serial.println(logString->buff); 
}

static void setFlushInterface(FlushInterface in)
{
    switch(in){
        case F_SERIAL:
            (logString->flushI) = printSerial;
            break;
        case F_WIFI:
            (logString->flushI) = printWiFiUDP;
            break; 
    }
}

LogString * createLogString(char * input, TLogLevel level)
{
    LogString * thisLog; 
    thisLog = static_cast<LogString*>(malloc( sizeof *thisLog ));
    if (NULL != thisLog)
    { 
        thisLog->length = strlen(input) + 1; 
        thisLog->messageLevel = level; 

        thisLog->buff = static_cast<char*>(malloc(thisLog->length));

        if( NULL != thisLog->buff)    
        {
            strncpy(thisLog->buff, input, thisLog->length); 
        }else{
            free(thisLog);
            thisLog = NULL; 
        }
    }
    return thisLog;
}

static void deleteLogString()
{
    free(logString->buff); 
    free(logString); 
    logString = NULL; 
}

static void appendLogString(char * const append)
{
    char baseString[logString->length + 1];
    strncpy(baseString, logString->buff, logString->length +1);  
    logString->length = strlen(logString->buff)  + strlen(append) + 1;  
    free(logString->buff); 
    logString->buff= static_cast<char*>(malloc (logString->length)); 
    if( NULL != logString->buff){
        logString->buff[0] = '\0';
        strcat(logString->buff, baseString); 
        strcat(logString->buff, append); 
    }
}

static TLogLevel GetReportingLevel(void)
{
    return currentLoglevel; 
}

static void GetLogString(TLogLevel level, char * msg)
{
    uint32_t currentTime = millis();
    char time[20];
    snprintf(time, 20, "%ld", currentTime); 
    char levelInText[]  = "\tLOG_VERBOSE\t"; 
    char logMessage [strlen(msg)+1]; 
    strncpy(logMessage, msg, strlen(msg)+1); 
    //TODO: Add real functions to retrieve the logLevelText and the RealTime

    logString = createLogString(time, logDEBUG); 
    appendLogString(levelInText); 
    appendLogString(logMessage); 
    setFlushInterface(F_SERIAL); 
    logString->flushI(); 
    deleteLogString(); 
}



LogI const logi = {GetLogString, GetReportingLevel, setFlushInterface}; 
