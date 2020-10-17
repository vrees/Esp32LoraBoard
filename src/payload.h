#ifndef _PAYLOAD_H_
#define _PAYLOAD_H_

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
typedef uint8_t byte;

class PayloadConvert {

public:
  PayloadConvert(uint8_t size);
  ~PayloadConvert();

  void reset(void);
  uint8_t getSize(void);
  uint8_t *getBuffer(void);
  void addByte(uint8_t value);
  void addCount(uint16_t value, uint8_t sniffytpe);
  void addConfig(configData_t value);
  void addStatus(uint16_t voltage, uint64_t uptime, float cputemp, uint32_t mem,
                 uint8_t reset1, uint8_t reset2);
  void addAlarm(int8_t rssi, uint8_t message);
  void addVoltage(uint16_t value);
  void addButton(uint8_t value);
private:
  void addChars( char* string, int len);


private:
  uint8_t *buffer;
  uint8_t cursor;
};

extern PayloadConvert payload;

#endif // _PAYLOAD_H_
