#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp32-lora-board-pins.h"

#define GPIO_OUTPUT_PIN_SEL ((1ULL << POWER_ENABLE) | (1ULL << U_EXT_ENABLE) | (1ULL << U_BAT_ENABLE))
#define GPIO_INPUT_PIN_SEL ((1ULL << U_EXT_MEASURE) | (1ULL << U_BAT_MEASURE))


void initIoPorts()
{
    // Configure all output pins
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // Configure all input pins
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void enablePeripheralPower()
{
    printf("Enabling Peripheral Power\n");
    gpio_set_level((gpio_num_t)POWER_ENABLE, 0);
}

void disablePeripheralPower()
{
    printf("Disabling Peripheral Power\n");
    gpio_set_level((gpio_num_t)POWER_ENABLE, 1);
}

void enableUbatMeasurement()
{
    printf("Enabling Battery voltage Measurement\n");
    gpio_set_level((gpio_num_t)U_BAT_ENABLE, 0);
}

void disableUbatMeasurement()
{
    printf("Disabling Battery voltage Measurement\n");
    gpio_set_level((gpio_num_t)U_BAT_ENABLE, 1);
}