#include "sensors.h"
#include "lighthouse_sensor.pb.h"

volatile static Sensor sensor_1; 
volatile static Sensor sensor_2; 
volatile static Sensor sensor_3; 

mkr1000_lighthouse_trackedObject_Sensor sensordata;

void rising_IRQ_S1(void)
{
    sensor_1.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void rising_IRQ_S2(void)
{
    sensor_2.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void rising_IRQ_S3(void)
{
    sensor_3.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void falling_IRQ_S1(void)
{
    sensor_1.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_1.mPulseWidthFIFO, (sensor_1.mStopT - sensor_1.mStartT)); 
}

void falling_IRQ_S2(void)
{
    sensor_2.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_2.mPulseWidthFIFO, (sensor_2.mStopT - sensor_2.mStartT)); 
}

void falling_IRQ_S3(void)
{
    sensor_3.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_3.mPulseWidthFIFO, (sensor_3.mStopT - sensor_3.mStartT)); 
}

void initSensors()
{
    // init the FIFO Buffers
    FIFO_init(sensor_1.mPulseWidthFIFO); 
    FIFO_init(sensor_2.mPulseWidthFIFO); 
    FIFO_init(sensor_3.mPulseWidthFIFO); 
}

 /*  (GCLK_SOURCE / GCLK_DIVIDE) / (PRESCALER + REGISTER COUNTS) = OVERFLOW FREQUENCE OF TCX
 *  (48Mhz / GCLK_DIVID) / PRESCALER + 2^16) = 2^16µS means
 *  Currenlty setted up to 1MHZ which equals count stamps of 1µS */
void initCounter()
{
    // divides the source frequence of the GLCK with the provided divide value
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |         
        GCLK_GENDIV_ID(4);           
    while (GCLK->STATUS.bit.SYNCBUSY);              

    // 48MHz source for the GCLK
    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |          
        GCLK_GENCTRL_GENEN |         
        GCLK_GENCTRL_SRC_DFLL48M |   
        GCLK_GENCTRL_ID(4);          
    while (GCLK->STATUS.bit.SYNCBUSY);           

    // passes GEN_GCLK4 to the counters TC4 and TC5
    REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |             
        GCLK_CLKCTRL_GEN_GCLK4 |                                
        GCLK_CLKCTRL_ID_TC4_TC5;                            
    while (GCLK->STATUS.bit.SYNCBUSY);                  
    REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT16;             
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);           

    REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV16|                 
        TC_CTRLA_ENABLE;                                    
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);          
}

// decode the detected Duration according to:
// https://github.com/nairol/LighthouseRedox/blob/master/docs/Light%20Emissions.md
static void decodeDuration(uint16_t duration)
{
    // which baseStation did emitted the sweep?
    if( 60 < duration && 100 > duration)
    {
        sensordata.sensors1[0]  = false; 
        sensordata.sensors0[0]  = true; 
        sensordata.angles0_h[0] = duration; 
        sensordata.angles0_v[0] = duration; 
   
    }

    else if( 100 < duration && 140 > duration)
    {
        sensordata.sensors1[0] = true; 
        sensordata.sensors0[0] = false; 
        sensordata.angles1_h[0] = duration; 
        sensordata.angles1_v[0] = duration; 

    }
    //TODO
    // #1 decide whether vertically or horizontally
    // #2 get the angle to the basestation from the sweep duration
    
}

static void saveDuration(SENSORTYPE sensor, uint16_t duration)
{
    sensordata.id= (int32_t) sensor; 
    sensordata.timestamp[0]= 0; 
    decodeDuration(duration); 
    
    // adds the decoded Sensor Data to the respective Sensor ProtoBuffer in the TrackedObject protob
    protoLove.addSensor_Data(&sensordata);  
}

void processSensorValues(void)
{
    uint16_t duration = 0; 

    duration = FIFO128_read(sensor_1.mPulseWidthFIFO); 
    while(duration != 0){
        saveDuration(SENSOR_1, duration);
        protoLove.incrementSensorEntry(); 
        duration = FIFO128_read(sensor_1.mPulseWidthFIFO); 
    }
    protoLove.resetSensorEntry(); 
    
    duration = FIFO128_read(sensor_2.mPulseWidthFIFO); 
    while(duration != 0){
        saveDuration(SENSOR_2, duration); 
        duration = FIFO128_read(sensor_2.mPulseWidthFIFO); 
        protoLove.incrementSensorEntry(); 
    }
    protoLove.resetSensorEntry(); 

    //duration = FIFO128_read(sensor_3.mPulseWidthFIFO); 
    //while(duration != 0){
    //    saveDuration(SENSOR_3, duration); 
    //    protoLove.incrementSensorEntry(); 
    //    duration = FIFO128_read(sensor_3.mPulseWidthFIFO); 
    //}
    //protoLove.resetSensorEntry(); 

    protoLove.encode_send_Proto();     
} 

SENSOR_LOVE const sensorlove = {rising_IRQ_S1, rising_IRQ_S2, rising_IRQ_S3, falling_IRQ_S1, falling_IRQ_S2, falling_IRQ_S3, initSensors, initCounter, decodeDuration, processSensorValues}; 
