#include "sensors.h"
#include "lighthouse_sensor.pb.h"

#define MU_PER_DEGREE 0.0216

volatile static Sensor sensor_1; 
volatile static Sensor sensor_2; 
volatile static Sensor sensor_3; 

volatile static uint16_t    s1_sweep_start, s1_sweep_stop; 
volatile static bool        s1_sweep_active; 
volatile static bool        s1_synced = false; 
volatile static Sweep       s1_sweep;  

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
    uint16_t duration = (sensor_1.mStopT - sensor_1.mStartT); 

    // sync pulse detected! Get duration and start sweep-counting measurment 
    if( duration > 50 )
    {
        // check if the sync pulse signals skip or not
        if((60 < duration && 70 > duration)
                || ( 81 < duration && 90 > duration))
        {
            LOG_d(logINFO, "Vertical sweep detected: ", duration); 

            s1_sweep.vertical = true; 

            s1_synced = true; 
            s1_sweep_active = true; 
            s1_sweep_start = sensor_1.mStartT; 

        }else if(s1_synced == false){
            LOG(logINFO, "skiped pulse to sync"); 
        }else if(( 71 < duration && 80 > duration)
                || ( 91 < duration && 100 > duration))
        {
            LOG_d(logINFO, "Horizontal sweep detected", duration); 

            s1_sweep.vertical = false; 

            s1_sweep_active = true; 
            s1_sweep_start = sensor_1.mStartT; 
        } 
    }
    // laser sweep detected! Complete sweep-counting measurement 
    else if(true == s1_sweep_active && duration < 50)
    {
        LOG_d(logDEBUG, "Laser sweep detected", duration); 
        s1_sweep_active = false; 
        s1_sweep.sweepDuration = sensor_1.mStartT - s1_sweep_start ; 

        Sweep * sweepWrite = static_cast<Sweep*>(malloc( sizeof(Sweep))); 
        if(sweepWrite != NULL){
            sweepWrite->sweepDuration   = s1_sweep.sweepDuration; 
            sweepWrite->vertical        = s1_sweep.vertical; 
        }
            
        FIFO128_write(sensor_1.mSweepFIFO, sweepWrite);  
    }
}

void falling_IRQ_S2(void)
{
    sensor_2.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    //FIFO128_write(sensor_2.mPulseWidthFIFO, (sensor_2.mStopT - sensor_2.mStartT)); 
}

void falling_IRQ_S3(void)
{
    sensor_3.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    //FIFO128_write(sensor_3.mPulseWidthFIFO, (sensor_3.mStopT - sensor_3.mStartT)); 
}

void initSensors()
{   
    sensor_1.id = 1; 
    sensor_2.id = 2; 
    sensor_3.id = 3; 

    // init the FIFO Buffers
    FIFO_init(sensor_1.mSweepFIFO); 
    FIFO_init(sensor_2.mSweepFIFO); 
    FIFO_init(sensor_3.mSweepFIFO); 
    enableLogging = false; 
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

static void processDuration(Sweep * detectedSweep)
{
    LOG(logVERBOSE_3, "process Sensor Values"); 
    uint8_t     res;

    uint8_t sweeptype = NONE ; 
    float angle = 0; 

    // decode the sync pulse length based on the reverse engineered protocol from https://github.com/nairol/LighthouseRedox/blob/master/docs/Light%20Emissions.md
    if(0 == detectedSweep->vertical)
    {
        sweeptype = VERTICAL; 
        angle = detectedSweep->sweepDuration * MU_PER_DEGREE; 
        LOG_f(logDEBUG,"Vertical Sweep detected", angle); 
    }else 
    {
        sweeptype = HORIZONTAL; 
        angle = detectedSweep->sweepDuration * MU_PER_DEGREE; 
        LOG_f(logDEBUG, "Horizontal sweep detected", angle); 
    }    

    // adds the decoded Sensor Data to the respective Sensor ProtoBuffer in the TrackedObject protob
    res = protoLove.addSensor_Data(angle, sweeptype);  
}

void processSensorValues(void)
{
    LOG(logDEBUG, "processing"); 
    LOG_d(logDEBUG, "process Sensor Values", 5); 
    Sweep * detectedSweep = FIFO128_read(sensor_1.mSweepFIFO); 
    while( NULL != detectedSweep){
        processDuration(detectedSweep);
        free(detectedSweep); 
        detectedSweep = FIFO128_read(sensor_1.mSweepFIFO); 
    }
    //protoLove.resetSensorEntry(); 
    //protoLove.encode_send_Proto(); 
} 

SENSOR_LOVE const sensorlove = {rising_IRQ_S1, rising_IRQ_S2, rising_IRQ_S3, falling_IRQ_S1, falling_IRQ_S2, falling_IRQ_S3, initSensors, initCounter, processSensorValues}; 
