#ifndef _SLEEP_WAKEUP_H_
#define _SLEEP_WAKEUP_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    TIMER_WAKEUP,                   // Wakeup by timer  
    LOW_LEVEL_WAKEUP,               // Water level sensor detected low level and initiated wakeup
    DEBUG_WAKEUP,                   // User pressed reset button
} operation_mode_t;

    extern operation_mode_t operation_mode;

    void wakeupAndInit();
    void powerOffAndSleep();

#ifdef __cplusplus
}
#endif

#endif