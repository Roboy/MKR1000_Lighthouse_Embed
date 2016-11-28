#include "sensors.h"
#include "lighthouse_sensor.pb.h"

#define MU_PER_DEGREE 0.0216

volatile static Sensor sensor_1; 
volatile static Sensor sensor_2; 
volatile static Sensor sensor_3; 

volatile static uint16_t    s1_sweep_start, s1_sweep_stop, s2_sweep_start, s2_sweep_stop, s3_sweep_start, s3_sweep_stop;  
volatile static bool        s1_sweep_active, s2_sweep_active, s3_sweep_active; 
volatile static bool        s1_synced = false, s2_synced = false, s3_synced = false;  
volatile static Sweep       s1_sweep, s2_sweep, s3_sweep;  

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
        LOG_d(logINFO, "Laser sweep detected", duration); 
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
    uint16_t duration = (sensor_2.mStopT - sensor_2.mStartT); 

    // sync pulse detected! Get duration and start sweep-counting measurment 
    if( duration > 50 )
    {
        // check if the sync pulse signals skip or not
        if((60 < duration && 70 > duration)
                || ( 81 < duration && 90 > duration))
        {
            LOG_d(logINFO, "Vertical sweep detected: ", duration); 

            s2_sweep.vertical = true; 
            s2_synced = true; 
            s2_sweep_active = true; 
            s2_sweep_start = sensor_2.mStartT; 

        }else if(s2_synced == false){
            LOG(logINFO, "skiped pulse to sync"); 
        }else if(( 71 < duration && 80 > duration)
                || ( 91 < duration && 100 > duration))
        {
            LOG_d(logINFO, "Horizontal sweep detected", duration); 

            s2_sweep.vertical = false; 

            s2_sweep_active = true; 
            s2_sweep_start = sensor_2.mStartT; 
        } 
    }
    // laser sweep detected! Complete sweep-counting measurement 
    else if(true == s2_sweep_active && duration < 50)
    {
        LOG_d(logINFO, "Laser sweep detected", duration); 
        s2_sweep_active = false; 
        s2_sweep.sweepDuration = sensor_2.mStartT - s2_sweep_start ; 

        Sweep * sweepWrite = static_cast<Sweep*>(malloc( sizeof(Sweep))); 
        if(sweepWrite != NULL){
            sweepWrite->sweepDuration   = s2_sweep.sweepDuration; 
            sweepWrite->vertical        = s2_sweep.vertical; 
        }
            
        FIFO128_write(sensor_2.mSweepFIFO, sweepWrite);  
    }
}

void falling_IRQ_S3(void)
{
    sensor_3.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    uint16_t duration = (sensor_3.mStopT - sensor_3.mStartT); 

    // sync pulse detected! Get duration and start sweep-counting measurment 
    if( duration > 50 )
    {
        // check if the sync pulse signals skip or not
        if((60 < duration && 70 > duration)
                || ( 81 < duration && 90 > duration))
        {
            LOG_d(logWARNING, "Vertical sweep detected: ", duration); 

            s3_sweep.vertical = true; 
            s3_synced = true; 
            s3_sweep_active = true; 
            s3_sweep_start = sensor_3.mStartT; 

        }else if(s3_synced == false){
            LOG(logWARNING, "skiped pulse to sync"); 
        }else if(( 71 < duration && 80 > duration)
                || ( 91 < duration && 100 > duration))
        {
            LOG_d(logWARNING, "Horizontal sweep detected", duration); 

            s3_sweep.vertical = false; 

            s3_sweep_active = true; 
            s3_sweep_start = sensor_3.mStartT; 
        } 
    }
    // laser sweep detected! Complete sweep-counting measurement 
    else if(true == s3_sweep_active && duration < 50)
    {
        LOG_d(logWARNING, "Laser sweep detected", duration); 
        s3_sweep_active = false; 
        s3_sweep.sweepDuration = sensor_3.mStartT - s3_sweep_start ; 

        Sweep * sweepWrite = static_cast<Sweep*>(malloc( sizeof(Sweep))); 
        if(sweepWrite != NULL){
            sweepWrite->sweepDuration   = s3_sweep.sweepDuration; 
            sweepWrite->vertical        = s3_sweep.vertical; 
        }
        FIFO128_write(sensor_3.mSweepFIFO, sweepWrite);  
    }
}


void initSensors()
{   
    sensor_1.id = 0; 
    sensor_2.id = 1; 
    sensor_3.id = 2; 

    // init the FIFO Buffers
    FIFO_init(sensor_1.mSweepFIFO); 
    FIFO_init(sensor_2.mSweepFIFO); 
    FIFO_init(sensor_3.mSweepFIFO); 
    enableLogging = true; 
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

static void processDuration(Sweep * detectedSweep, uint16_t id)
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
        LOG_f(logINFO,"Vertical Sweep detected", angle); 
    }else 
    {
        sweeptype = HORIZONTAL; 
        angle = detectedSweep->sweepDuration * MU_PER_DEGREE; 
        LOG_f(logINFO, "Horizontal sweep detected", angle); 
    }    

    // adds the decoded Sensor Data to the respective Sensor ProtoBuffer in the TrackedObject protob
    res = protoLove.addSensor_Data(angle, sweeptype, id);  
}

void processSensorValues(void)
{
    Sweep * detectedSweep = FIFO128_read(sensor_1.mSweepFIFO); 
    while( NULL != detectedSweep){
        processDuration(detectedSweep, sensor_1.id);
        free(detectedSweep); 
        detectedSweep = FIFO128_read(sensor_1.mSweepFIFO); 
    }

    detectedSweep = FIFO128_read(sensor_2.mSweepFIFO); 
    while( NULL != detectedSweep){
        processDuration(detectedSweep, sensor_2.id);
        free(detectedSweep); 
        detectedSweep = FIFO128_read(sensor_2.mSweepFIFO); 
    }

    detectedSweep = FIFO128_read(sensor_3.mSweepFIFO); 
    while( NULL != detectedSweep){
        processDuration(detectedSweep, sensor_3.id);
        free(detectedSweep); 
        detectedSweep = FIFO128_read(sensor_3.mSweepFIFO); 
    }
} 

SENSOR_LOVE const sensorlove = {rising_IRQ_S1, rising_IRQ_S2, rising_IRQ_S3, falling_IRQ_S1, falling_IRQ_S2, falling_IRQ_S3, initSensors, initCounter, processSensorValues}; 
