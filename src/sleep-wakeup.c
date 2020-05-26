
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp32-lora-board-pins.h"

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
int sleepTimeInSeconds = 15;   /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

#define UBAT_ENABLE (25)

void xxxxx()
{
  printf("Hello world!\n");
  gpio_pad_select_gpio(UBAT_ENABLE);
  gpio_set_direction((gpio_num_t)UBAT_ENABLE, GPIO_MODE_OUTPUT);

  gpio_set_intr_type((gpio_num_t)UBAT_ENABLE, GPIO_INTR_ANYEDGE);

  /* Blink off (output low) */
  printf("Turning off the LED\n");
  gpio_set_level((gpio_num_t)UBAT_ENABLE, 0);
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  /* Blink on (output high) */
  printf("Turning on the LED\n");
  gpio_set_level((gpio_num_t)UBAT_ENABLE, 1);
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  printf("Restarting now.\n");
  fflush(stdout);
  gpio_deep_sleep_hold_en();
  gpio_hold_en((gpio_num_t)UBAT_ENABLE);
  esp_sleep_enable_timer_wakeup(10000000);
  esp_deep_sleep_start();
}

/*
Method to print the reason by which ESP32
*/
void print_wakeup_reason()
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

void wakeupAndSleep()
{
  printf("BEGIN: wakeupAndSleep()\n");

  //Increment boot number and print it every reboot
  ++bootCount;
  printf("Boot number: %d\n", bootCount);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  vTaskDelay(9000 / portTICK_PERIOD_MS);
  ; //Take some time to measure current

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 15 seconds
  */
  esp_sleep_enable_timer_wakeup(sleepTimeInSeconds * uS_TO_S_FACTOR);
  printf("Setup ESP32 to sleep for every %i in Seconds\n", sleepTimeInSeconds);

  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //printf("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  printf("Going to sleep now\n");
  esp_deep_sleep_start();
  printf("This will never be printed");
}
