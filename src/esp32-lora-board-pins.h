#ifndef _ESP32_LORA_BOARD_PINS_H_
#define _ESP32_LORA_BOARD_PINS_H_

// Pins for LORA chip SPI interface, reset line and interrupt lines (compatible TTGO LoRa32 V2.1.6)
// #define LORA_SCK  (5) 
// #define LORA_CS   (18)
// #define LORA_MISO (19)
// #define LORA_MOSI (27)
// #define LORA_RST  (23)
// #define LORA_IRQ  (26)
// #define LORA_IO1  (33)
// #define LORA_IO2  (32)

// Pins and other resources
#define TTN_SPI_HOST      HSPI_HOST
#define TTN_SPI_DMA_CHAN  1
#define TTN_PIN_SPI_SCLK  5
#define TTN_PIN_SPI_MOSI  27
#define TTN_PIN_SPI_MISO  19
#define TTN_PIN_NSS       18
#define TTN_PIN_RXTX      TTN_NOT_CONNECTED
#define TTN_PIN_RST       23
#define TTN_PIN_DIO0      26
#define TTN_PIN_DIO1      33


// JTAG ESP-Programmer
#define JTAG_TDI        (12)
#define JTAG_TCK        (13)
#define JTAG_TMS        (14)
#define JTAG_TDO        (15)

// Control Pins
#define POWER_ENABLE    (16)
#define U_EXT_ENABLE    (4)
#define U_EXT_MEASURE   (35)    // ADC1_CHANNEL_7
#define U_BAT_ENABLE     (25)
#define U_BAT_MEASURE    (34)   // ADC1_CHANNEL_6

// IO
#define ROTARY_SWITCH   (17)
#define ROTARY_A        (36)
#define ROTARY_B        (39)
#define RESERVED        (2)

// I2C Bus
#define I2C_SDA         (21)
#define I2C_SCL         (22)

// Serial Connection
#define TXDO            (1)
#define RXDO            (3)

#endif