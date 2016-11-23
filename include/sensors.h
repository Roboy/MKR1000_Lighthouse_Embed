#ifndef LIGHTHOUSETIMER_H
#define LIGHTHOUSETIMER_H

#include "board.h"
#include "fifo.h"

//void rising_IRQ_S1(void); 
//void falling_IRQ_S1(void); 

typedef struct _sweepTime{
    uint16_t    startC; 
    bool        active;  
}sweepTime; 

typedef struct _Sensor{
    uint16_t    id; 
    uint16_t    mStartT; 
    uint16_t    mStopT; 
    FIFO128t    mPulseWidthFIFO; 
    FIFO128t    mSweepCountFIFO; 
    FIFO128t    mVerticalFIFO; 
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
#endif // LIGHTHOUSETIMER_H
