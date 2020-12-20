#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include <driver/spi_master.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_sleep.h"
#include "TheThingsNetwork.h"

#include "esp32-lora-board-pins.h"
#include "sleep-wakeup.h"
#include "voltage.h"
#include "u8g2.h"

#include "sdkconfig.h"

#include "u8g2_esp32_hal.h"

// extern CayenneLPP lpp;
extern uint8_t payload[];

static TheThingsNetwork ttn;

// TTN-App: public-dummy

// debug board zum Testen
// MAC: 3C:71:BF:4C:FC:18
const char *appEui = "70B3D57ED002FB99";
const char *appKey = "E2A4A1D6D67ADD8D539D13DC985F369E";

const unsigned TX_INTERVAL = 5;

// SDA - GPIO21
#define PIN_SDA GPIO_NUM_21

// SCL - GPIO22
#define PIN_SCL GPIO_NUM_22

static const char *TAG = "ssd1306";

void task_test_SSD1306i2c(void *ignore)
{
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.sda = PIN_SDA;
    u8g2_esp32_hal.scl = PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_t u8g2; // a structure which will contain all the data for one display
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        &u8g2,
        U8G2_R0,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

    ESP_LOGI(TAG, "u8g2_SetPowerSave");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    ESP_LOGI(TAG, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);
    ESP_LOGI(TAG, "u8g2_DrawBox");
    u8g2_DrawBox(&u8g2, 0, 26, 80, 6);
    u8g2_DrawFrame(&u8g2, 0, 26, 100, 6);

    ESP_LOGI(TAG, "u8g2_SetFont");
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    ESP_LOGI(TAG, "u8g2_DrawStr");
    u8g2_DrawStr(&u8g2, 2, 17, "Hi nkolban!");
    ESP_LOGI(TAG, "u8g2_SendBuffer");
    u8g2_SendBuffer(&u8g2);

    ESP_LOGI(TAG, "All done!");

    // vTaskDelete(NULL);
}

void initEsp32Resources()
{
    esp_err_t err;
    // Initialize the GPIO ISR handler service
    err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    ESP_ERROR_CHECK(err);

    // Initialize the NVS (non-volatile storage) for saving and restoring the keys
    err = nvs_flash_init();
    ESP_ERROR_CHECK(err);

    // Initialize SPI bus
    spi_bus_config_t spi_bus_config;
    spi_bus_config.miso_io_num = TTN_PIN_SPI_MISO;
    spi_bus_config.mosi_io_num = TTN_PIN_SPI_MOSI;
    spi_bus_config.sclk_io_num = TTN_PIN_SPI_SCLK;
    spi_bus_config.quadwp_io_num = -1;
    spi_bus_config.quadhd_io_num = -1;
    spi_bus_config.max_transfer_sz = 0;
    err = spi_bus_initialize(TTN_SPI_HOST, &spi_bus_config, TTN_SPI_DMA_CHAN);
    ESP_ERROR_CHECK(err);
}

void printRFSettings(const char *window, const TTNRFSettings &settings)
{
    int bw = (1 << (static_cast<int>(settings.bandwidth) - 1)) * 125;
    int sf = static_cast<int>(settings.spreadingFactor) + 5;

    if (settings.spreadingFactor == kTTNSFNone)
    {
        printf("%s: not used\n", window);
    }
    else if (settings.spreadingFactor == kTTNFSK)
    {
        printf("%s: FSK, BW %dkHz, %d.%d MHz\n",
               window, bw, settings.frequency / 1000000, (settings.frequency % 1000000 + 50000) / 100000);
    }
    else
    {
        printf("%s: SF%d, BW %dkHz, %d.%d MHz\n",
               window, sf, bw, settings.frequency / 1000000, (settings.frequency % 1000000 + 50000) / 100000);
    }
}

void printAllRFSettings()
{
    printRFSettings("TX ", ttn.txSettings());
    printRFSettings("RX1", ttn.rx1Settings());
    printRFSettings("RX2", ttn.rx2Settings());
    printf("RSSI: %d dBm\n", ttn.rssi());
}

void sendMessages(void *pvParameter)
{
    // rintf("Sending message: %s\n", uploadMessage);
    TTNResponseCode res = ttn.transmitMessage(payload, PAYLOAD_LENGTH);
    printf(res == kTTNSuccessfulTransmission ? "Message sent.\n" : "Transmission failed.\n");

    printAllRFSettings();

    vTaskDelay(TX_INTERVAL * 1000 / portTICK_PERIOD_MS);

    ttn.shutdown();
    powerOffAndSleep();
}

void messageReceived(const uint8_t *message, size_t length, port_t port)
{
    printf("*********************************************\n");
    printf("Message of %d bytes received on port %d:", length, port);
    for (int i = 0; i < length; i++)
        printf(" %02x", message[i]);
    printf("\n");
    printf("*********************************************\n");
}

void showMacAddress()
{
    uint8_t mac[6];
    esp_err_t err = esp_efuse_mac_get_default(mac);
    ESP_ERROR_CHECK(err);
    printf("MAC-Adress:   ");
    for (int i = 0; i < 6; i++)
        printf(" %02x", mac[i]);
    printf("\n");
}

extern "C" void app_main(void)
{
    printf("Start app on ESP32LoraBoard\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS); //Take some time to open up the Serial Monitor

    wakeupAndInit();

    initEsp32Resources();

    task_test_SSD1306i2c(NULL);

    // Configure the SX127x pins
    ttn.configurePins(TTN_SPI_HOST, TTN_PIN_NSS, TTN_PIN_RXTX, TTN_PIN_RST, TTN_PIN_DIO0, TTN_PIN_DIO1);

    showMacAddress();
    ttn.provisionWithMAC(appEui, appKey);

    // Register callback for received messages
    ttn.onMessage(messageReceived);

    readSensorValues();

    printf("Joining...\n");
    if (ttn.join())
    {
        printf("Joined.\n");
        xTaskCreate(sendMessages, "send_messages", 1024 * 4, (void *)0, 3, nullptr);
    }
    else
    {
        printf("Join failed. Goodbye\n");
    }
}