# Esp32LoraBoard
ESP32 board including Lora RFM95 module optimized for low power deep sleep operation.

![PCB 3D](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_3D_V1.1.png)

[Schema](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_Schema_V1.1.pdf)

[PCB 3D](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_3D_V1.1.png)

[PCB](../master/kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_V1.1.png)



Checkout ttn-esp32 into lib subdir. See https://github.com/manuelbl/ttn-esp32. Switch to branch dev to get it running.


## Power consumption

### Power on and idle 

| Mode              | Current usage |
| ----------------- |:-------------:|
| Normal operation  | 28 mA         |


### Deep Sleep 

Switched off all peripheral components and shutdown RFM95W.

| Mode              | Current usage |
| ----------------- |:-------------:|
| Deep Sleep        | 20 uA         |

