#ifndef SENSORS_H
#define SENSORS_H

#include "board.h"

typedef struct _Sensor{
    uint16_t        id; 
    uint16_t        mStartT; 
    uint16_t        mStopT; 
    FIFO128sweep    mSweepFIFO; 
}Sensor; 

typedef struct _SENSOR_LOVE{
    void    (* const    rising_IRQ_S1)(void);
    void    (* const    rising_IRQ_S2)(void);
    void    (* const    rising_IRQ_S3)(void);
    void    (* const    falling_IRQ_S1)(void);
    void    (* const    falling_IRQ_S2)(void);
    void    (* const    falling_IRQ_S3)(void);
    void    (* const    initSensors)(void); 
    void    (* const    initCounter)(void);
    void    (* const    processSensorValues)(void); 
}SENSOR_LOVE;

extern SENSOR_LOVE const sensorlove; 
#endif // SENSORS_
