// Basic Config
#include "senddata.h"

Ticker sendcycler;

void sendcycle()
{
  xTaskNotifyFromISR(irqHandlerTask, SENDCYCLE_IRQ, eSetBits, NULL);
}

// put data to send in RTos Queues used for transmit over channels Lora and SPI
void SendPayload(uint8_t port, sendprio_t prio)
{

  MessageBuffer_t
      SendBuffer; // contains MessageSize, MessagePort, MessagePrio, Message[]

  SendBuffer.MessageSize = payload.getSize();
  SendBuffer.MessagePrio = prio;

  SendBuffer.MessagePort = port;
  memcpy(SendBuffer.Message, payload.getBuffer(), SendBuffer.MessageSize);

  // enqueue message in device's send queues
  lora_enqueuedata(&SendBuffer);
} // SendPayload

// interrupt triggered function to prepare payload to send
void sendData()
{

  uint8_t bitmask = cfg.payloadmask;
  uint8_t mask = 1;

  while (bitmask)
  {
    switch (bitmask & mask)
    {
    case GPS_DATA:
      if (GPSPORT != COUNTERPORT)
      {
        // send GPS position only if we have a fix
        if (gps_hasfix())
        {
          gps_storelocation(&gps_status);
          payload.reset();
          payload.addGPS(gps_status);
          SendPayload(GPSPORT, prio_high);
        }
        else
          ESP_LOGD(TAG, "No valid GPS position");
      }
      break;
    } // switch
    bitmask &= ~mask;
    mask <<= 1;
  } // while (bitmask)

} // sendData()

void flushQueues()
{
#if (HAS_LORA)
  lora_queuereset();
#endif
}
