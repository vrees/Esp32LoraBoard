# Esp32LoraBoard
ESP32 board including Lora RFM95 module optimized for low power deep sleep operation.

[Schema](../kicad/images_V1.1/Esp32LoraBoard_Kicad_Schema_V1.1.pdf)
[PCB 3D](../kicad/images_V1.1/Esp32LoraBoard_Kicad_PCB_3D_V1.1.png)
[PCB](../kicad/images_V1.1/ 	Esp32LoraBoard_Kicad_PCB_V1.1.png)


## Power consumption

### All Power FETs disabled during startup
no further activity is done yet to reduce RFM95W power
Normal operation:   28 mA
Deep Leep:          1.7 mA

| Mode              | Current usage |
| ----------------- |:-------------:|
| Normal operation  | 28 mA         |
| Deep Leep         | 1.7 mA        |