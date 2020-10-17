#include "globals.h"
#include "payload.h"

PayloadConvert::PayloadConvert(uint8_t size)
{
  buffer = (uint8_t *)malloc(size);
  cursor = 0;
}

PayloadConvert::~PayloadConvert(void) { free(buffer); }

void PayloadConvert::reset(void) { cursor = 0; }

uint8_t PayloadConvert::getSize(void) { return cursor; }

uint8_t *PayloadConvert::getBuffer(void) { return buffer; }

/* ---------------- plain format without special encoding ---------- */

void PayloadConvert::addByte(uint8_t value) { buffer[cursor++] = (value); }

void PayloadConvert::addCount(uint16_t value, uint8_t snifftype)
{
  buffer[cursor++] = highByte(value);
  buffer[cursor++] = lowByte(value);
}

void PayloadConvert::addAlarm(int8_t rssi, uint8_t msg)
{
  buffer[cursor++] = rssi;
  buffer[cursor++] = msg;
}

void PayloadConvert::addVoltage(uint16_t value)
{
  buffer[cursor++] = highByte(value);
  buffer[cursor++] = lowByte(value);
}

void PayloadConvert::addConfig(configData_t value)
{
  buffer[cursor++] = value.txpower;
  buffer[cursor++] = value.screensaver;
  buffer[cursor++] = value.screenon;
  buffer[cursor++] = value.sendcycle;
  memcpy(buffer + cursor, value.version, 10);
  cursor += 10;
}

void PayloadConvert::addStatus(uint16_t voltage, uint64_t uptime, float cputemp,
                               uint32_t mem, uint8_t reset1, uint8_t reset2)
{

  buffer[cursor++] = highByte(voltage);
  buffer[cursor++] = lowByte(voltage);
  buffer[cursor++] = (byte)((uptime & 0xFF00000000000000) >> 56);
  buffer[cursor++] = (byte)((uptime & 0x00FF000000000000) >> 48);
  buffer[cursor++] = (byte)((uptime & 0x0000FF0000000000) >> 40);
  buffer[cursor++] = (byte)((uptime & 0x000000FF00000000) >> 32);
  buffer[cursor++] = (byte)((uptime & 0x00000000FF000000) >> 24);
  buffer[cursor++] = (byte)((uptime & 0x0000000000FF0000) >> 16);
  buffer[cursor++] = (byte)((uptime & 0x000000000000FF00) >> 8);
  buffer[cursor++] = (byte)((uptime & 0x00000000000000FF));
  buffer[cursor++] = (byte)(cputemp);
  buffer[cursor++] = (byte)((mem & 0xFF000000) >> 24);
  buffer[cursor++] = (byte)((mem & 0x00FF0000) >> 16);
  buffer[cursor++] = (byte)((mem & 0x0000FF00) >> 8);
  buffer[cursor++] = (byte)((mem & 0x000000FF));
  buffer[cursor++] = (byte)(reset1);
  buffer[cursor++] = (byte)(reset2);
}




void PayloadConvert::addButton(uint8_t value)
{
#ifdef HAS_BUTTON
  buffer[cursor++] = value;
#endif
}


void PayloadConvert::addChars(char *string, int len)
{
  for (int i = 0; i < len; i++)
    addByte(string[i]);
}
