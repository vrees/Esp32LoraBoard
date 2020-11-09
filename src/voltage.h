#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_


#ifdef __cplusplus
extern "C"
{
#endif
    
    typedef enum
    {
        LOW = 0,
        HIGH = 1,
    } water_level_t;


    void readSensorValues();
    water_level_t getWaterLevel();

    // extern uint8_t uploadMessage[];

#ifdef __cplusplus
}
#endif

#endif