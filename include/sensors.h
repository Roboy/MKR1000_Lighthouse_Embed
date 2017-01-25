#ifndef SENSORS_H
#define SENSORS_H

#include "board.h"

typedef struct _Sensor{
    FIFO128t    mSweepFIFO; 
}Sensor; 

typedef struct _SENSOR_LOVE{
    void    (* const    sensor_spi)(void);
    void    (* const    initSensors)(void); 
    void    (* const    processSensorValues)(void); 
}SENSOR_LOVE;

extern SENSOR_LOVE const sensorlove; 
#endif // SENSORS_
