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

static const char *TAG = "ssd1306";

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

    // draw box
    // u8g2_DrawBox(&u8g2, 0, 26, 30,6);
    // u8g2_DrawFrame(&u8g2, 0,26,100,6);

    u8g2_SetFont(&u8g2, u8g2_font_8x13B_mr);  // 10 Pixel
    // u8g2_DrawStr(&u8g2, 1, 11, "Hallo");

    u8g2_SendBuffer(&u8g2);
}

void displayData()
{
    u8g2_ClearBuffer(&u8g2);

    u8g2_DrawStr(&u8g2, 0, 10, "1");     // sensor_values.vccVoltage);
    u8g2_DrawStr(&u8g2, 0, 20, "2");     // sensor_values.vccVoltage);
    u8g2_DrawStr(&u8g2, 0, 30, "3");     // sensor_values.vccVoltage);
    u8g2_DrawStr(&u8g2, 0, 40, "Hallo"); // sensor_values.vccVoltage);

    u8g2_SendBuffer(&u8g2);
}
