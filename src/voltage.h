#ifndef _VOLTAGE_H_
#define _VOLTAGE_H_
#include "CayenneLPP.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern CayenneLPP lpp;
    void readSensorValues();

    // extern uint8_t uploadMessage[];

#ifdef __cplusplus
}
#endif
#endif