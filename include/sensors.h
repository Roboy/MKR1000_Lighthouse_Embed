#ifndef LIGHTHOUSETIMER_H
#define LIGHTHOUSETIMER_H

#include "board.h"

typedef struct _SENSOR_LOVE{
    void    (* const    rising_IRQ_S1)(void);
    void    (* const    rising_IRQ_S2)(void);
    void    (* const    rising_IRQ_S3)(void);
    void    (* const    fallgin_IRQ_S1)(void);
    void    (* const    fallgin_IRQ_S2)(void);
    void    (* const    fallgin_IRQ_S3)(void);
    void    (* const    initSensors)(void); 
    void    (* const    initCounter)(void);
    void    (* const    decodeDuration)(uint16_t duration); 
    void    (* const    processSensorValues)(void); 
}SENSOR_LOVE;

extern SENSOR_LOVE const sensorlove; 
#endif // LIGHTHOUSETIMER_H
