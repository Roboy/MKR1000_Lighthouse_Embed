#ifndef LIGHTHOUSETIMER_H
#define LIGHTHOUSETIMER_H

#include "Arduino.h"
#include "WirelessLove.h"

typedef enum _SENSORTYPE{
    SENSOR_1 = 1,
    SENSOR_2, 
    SENSOR_3,
}SENSORTYPE; 

typedef struct SensorData{
    int32_t id; 
    int32_t timestamp;
    bool    sensor0;
    float   angel0_h; 
    float   angel0_v;
    bool    sensor1;
    float   angel1_h;
    float   angel1_v;
}SensorData; 

/**
 * see mikrocontroller forum tutorial 
 * */
typedef struct _FIFO128t{
    uint8_t     mRead;
    uint8_t     mWrite;
    uint16_t    mBuffer[128]; 
}FIFO128t; 

typedef struct _Sensor{
    uint16_t    mStartT; 
    uint16_t    mStopT; 
    FIFO128t    mPulseWidthFIFO; 
}Sensor; 


//FIFO Operations are implemented as Preprocessor Functions since the FIFO is used in several IRQs an things need to get a bit faster
#define FIFO_init(fifo)                 { fifo.mRead = 0; fifo.mWrite = 0;}

#define FIFO_available(fifo)            ( fifo.mRead != fifo.mWrite )

#define FIFO_read(fifo, size)           (                                           \
        (FIFO_available(fifo)) ?                                                    \
        fifo.mBuffer[(fifo.mRead = ((fifo.mRead + 1) & (size -1)))] : 0             \
        ) 

#define FIFO_write(fifo, data, size)    {                                           \
    uint8_t temp = ((fifo.mWrite +1 )& (size -1));                                  \
    if(temp != fifo.mRead) {                                                        \
        fifo.mBuffer[temp] = data;                                                  \
        fifo.mWrite = temp;                                                         \
    }                                                                               \
}

#define FIFO128_read(fifo)              FIFO_read(fifo, 128)
#define FIFO128_write(fifo, data)       FIFO_write(fifo, data, 128)

/**
 * @brief IRQ of the Sensors, called from the main function 
 **/
void rising_IRQ_S1(void); 
void rising_IRQ_S2(void); 
void rising_IRQ_S3(void); 

void falling_IRQ_S1(void); 
void falling_IRQ_S2(void); 
void falling_IRQ_S3(void); 

/**
 * @brief initializes the FIFO Buffers of the Sensor
 * */
void initSensors(); 

bool decodeDuration(int duration, bool& skip, bool& rotor, bool& data); 

/**
 * @brief writes directly to the registers of the SAMD21 in order to set up counter with a defined frequency to generate timestamps of the incoming sensor events
 * */
void initTimer();

/**
 * @brief initializes the interrupt requests function pointers of the Sensors
 * */
void initSensor(); 

void processSensorValues();

#endif // LIGHTHOUSETIMER_H
