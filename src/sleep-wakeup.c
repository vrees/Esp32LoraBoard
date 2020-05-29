
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp32-lora-board-pins.h"
#include "power.h"

#ifdef __cplusplus
}
#endif

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */

int sleepTimeInSeconds = 20; /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;

/*
Method to print the reason by which ESP32
*/
void printWakeupReason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    printf("Wakeup caused by external signal using RTC_IO\n");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    printf("Wakeup caused by external signal using RTC_CNTL\n");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    printf("Wakeup caused by timer\n");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    printf("Wakeup caused by touchpad\n");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    printf("Wakeup caused by ULP program\n");
    break;
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    printf("Wakeup was not caused by deep sleep\n");
    break;
  default:
    printf("Wakeup was not caused by deep sleep\n");
    break;
  }
}

void wakeupAndInit()
{
  //Increment boot number and print it every reboot
  ++bootCount;
  printf("wakeup(). Boot number: %d\n", bootCount);

  //Print the wakeup reason for ESP32
  printWakeupReason();

  // define output level before port config to ensure unwanted glitch
  enablePeripheralPower();
  enableBatteryVoltageMeasurement();
  enableExternalVoltageMeasurement();
  initIoPorts();
}

void powerOffAndSleep()
{
  printf("Preparing for deep sleep\n");

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  disablePeripheralPower();
  disableExternalVoltageMeasurement();
  disableBatteryVoltageMeasurement();
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  esp_sleep_enable_timer_wakeup(sleepTimeInSeconds * uS_TO_S_FACTOR);
  printf("Setup ESP32 to sleep for every %i in Seconds\n", sleepTimeInSeconds);

  
  printf("Going to sleep now\n");
  fflush(stdout);
  esp_deep_sleep_start();
  printf("This will never be printed");
}

#ifdef __cplusplus
}
#endif