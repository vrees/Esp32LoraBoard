#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "esp32-lora-board-pins.h"
#include "u8g2.h"
#include "u8g2_esp32_hal.h"
#include "voltage.h"
#include "sleep-wakeup.h"

static const char *TAG = "ssd1306";

char buff[32];

u8g2_t u8g2; // a structure which will contain all the data for one display

void initSSD1306i2c(void *ignore)
{
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.sda = I2C_SDA;
    u8g2_esp32_hal.scl = I2C_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        &u8g2,
        U8G2_R0,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x13_mr); // 10 Pixel

    u8g2_SendBuffer(&u8g2);
}

void displayData()
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_mr); // 8 Pixel

    sprintf(buff, "%1.2f V         %6d", sensor_values.vccVoltage, sensor_values.bootCount);
    u8g2_DrawStr(&u8g2, 0, 8, buff);           // sensor_values.vccVoltage);
    u8g2_DrawStr(&u8g2, 0, 22, "Wasser:"); // sensor_values.vccVoltage);

    u8g2_SetFont(&u8g2, u8g2_font_10x20_mr); // 13 Pixel
    if (sensor_values.waterLevel == 0)
        u8g2_DrawStr(&u8g2, 54, 26, "normal"); // sensor_values.vccVoltage);
    else
        u8g2_DrawStr(&u8g2, 56, 28, "niedrig"); // sensor_values.vccVoltage);

    u8g2_SendBuffer(&u8g2);
}
