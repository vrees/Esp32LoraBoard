# Esp32LoraBoard
ESP32 board including Lora RFM95 module optimized for low power deep sleep operation. 

## Features:
* works with single LiFePo battery
* external peripheral (i2c bus, Oled, measurement resistors) can be switxched off by FET transistors
* Open Source. Feel free to created your own modifications in KiCad and order your own boards
* I2C bus is decoubled be Levelshifter 
* optional power supply circuit (solder if you don't want do use LiFePo battery supply)
* optional Rotary Encoder 
* there is no UART chip (like CP2104) onboard in order to save power. Board can be programmed by using a simple external USB-to-UART converter (5 Pins: Ground, RX, TX RTS, DTR)



![Esp32LoraBoard](../master/kicad/images_V1.1/Esp32LoraBoard-DeepSleep.png)

![PCB 3D](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_3D_V1.1.png)

[Schema](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_Schema_V1.1.pdf)

[PCB](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_V1.1.png)



Checkout ttn-esp32 into lib subdir of this platformio project. See https://github.com/manuelbl/ttn-esp32. Switch to branch dev to get it running.

Change following:
__src/lmic/config.h__
#define USE_ORIGINAL_AES

__src/aes/lmic_aes.c__
line 260:  u4_t t0, t1=0, t2, t3;



### Power consumption

#### Power on and idle 

After PowerOn without sending any Lora data

| Mode              | Current usage |
| ----------------- |:-------------:|
| Normal operation  | 28 mA         |


#### Deep Sleep 

Switched off all peripheral components and shutdown the RFM95W lora chip.

| Mode              | Current usage |
| ----------------- |:-------------:|
| Deep Sleep        | 20 uA         |

