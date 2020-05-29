#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "TheThingsNetwork.h"

#include "esp32-lora-board-pins.h"
#include "sleep-wakeup.h"

static TheThingsNetwork ttn;

// TTN-App: public-dummy
const char *devEui = "008929224D146086";
const char *appEui = "70B3D57ED002FB99";
const char *appKey = "28F7CCAD7AFE1643EC96B7F52E145699";

const unsigned TX_INTERVAL = 5;
static uint8_t msgData[] = "Hello, world";

void sendMessages(void *pvParameter)
{
    printf("Sending message...\n");
    TTNResponseCode res = ttn.transmitMessage(msgData, sizeof(msgData) - 1);
    printf(res == kTTNSuccessfulTransmission ? "Message sent.\n" : "Transmission failed.\n");
    vTaskDelay(TX_INTERVAL * 1000 / portTICK_PERIOD_MS);

    ttn.shutdown();
    powerOffAndSleep();
}

extern "C" void app_main(void)
{
    printf("Hello world! ****************************************************************\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS); //Take some time to open up the Serial Monitor

    wakeupAndInit();

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

    // Configure the SX127x pins
    ttn.configurePins(TTN_SPI_HOST, TTN_PIN_NSS, TTN_PIN_RXTX, TTN_PIN_RST, TTN_PIN_DIO0, TTN_PIN_DIO1);

    // The below line can be commented after the first run as the data is saved in NVS
    ttn.provision(devEui, appEui, appKey);

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