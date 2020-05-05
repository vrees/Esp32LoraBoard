#include <Arduino.h>
#include "esp32-lora-board-pins.h"
#include "sleep-wakeup.h"



void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  wakeupAndSleep();
}

void loop() {

}