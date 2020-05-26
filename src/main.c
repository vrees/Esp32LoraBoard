#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
// #include "TheThingsNetwork.h"

#include "esp32-lora-board-pins.h"
#include "sleep-wakeup.h"

void handleWakpeup(void *pvParameter)
{
    wakeupAndSleep();
}

void app_main()
{
    printf("Hello world! ****************************************************************\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS); //Take some time to open up the Serial Monitor

    printf("Will start main thread.\n");
    xTaskCreate(handleWakpeup, "handleWakpeup", 1024 * 4, (void *)0, 3, NULL);
}