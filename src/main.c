#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
// #include "TheThingsNetwork.h"

#include "esp32-lora-board-pins.h"
#include "sleep-wakeup.h"

void app_main()
{
    vTaskDelay(1000 / portTICK_PERIOD_MS); //Take some time to open up the Serial Monitor

    wakeupAndSleep();
}