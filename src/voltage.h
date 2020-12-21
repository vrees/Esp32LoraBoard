#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PAYLOAD_LENGTH 6

    typedef enum
    {
        LOW = 0,
        HIGH = 1,
    } water_level_t;

    typedef struct
    {
        float c0;
        float c1;
        float c2;
        float c3;
        float c4;
    } polynom_coeffients_t;

    typedef struct
    {
        water_level_t waterLevel;
        float vccVoltage;
        int bootCount;
    } sensor_values_t;

    void readSensorValues();
    water_level_t getWaterLevel();

    extern uint8_t payload[PAYLOAD_LENGTH];
    extern sensor_values_t sensor_values;

#ifdef __cplusplus
}
#endif

#endif