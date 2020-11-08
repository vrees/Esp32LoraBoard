#include <driver/adc.h>
#include <math.h>
#include "esp32-lora-board-pins.h"
#include "CayenneLPP.h"
#include "voltage.h"

#ifdef __cplusplus
extern "C"
{
#endif

  int number_round = 100;
  int adc_reading_42V = 0;
  int adc_reading_33V = 0;
  /*
  uint8_t uploadMessage[2];

  void prepareVoltageMessage(float voltage)
  {
    int16_t volt = voltage * 1000;

    uploadMessage[0] = ((volt & 0xFF00) >> 8);
    uploadMessage[1] = volt & 0x00FF;

    printf("uploadMessage=%02X:%02X\n", uploadMessage[0], uploadMessage[1]);   
  }
*/
  CayenneLPP lpp(20);

  void initVoltage()
  {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_6); // VCC Voltage
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_6); // external Voltage
  }

  int readRoundedAdc(adc1_channel_t channel)
  {
    printf("ADC: (Channel=%d)\n", channel);
    int adc;
    int adc_sum = 0;

    for (size_t i = 0; i < number_round; i++)
    {
      adc = adc1_get_raw(channel);

      adc_sum += adc;
      printf("%-4d\t", adc);
      if ((i % 20) == 19)
        printf("\n");
    }

    adc_sum = adc_sum / number_round;
    printf("Rounded ADC value=%-4d\n", adc_sum);

    return adc_sum;
  }

  double calulateVoltageCompensated(double adc)
  {
    /* 
    Calulate ploynom http://www.xuru.org/rt/PR.asp
    y = 5.814402272·10-2 x4 - 6.614275168·10-1 x3 + 2.785160725 x2 - 4.077128492 x + 3.802638788

    Polynom bestimmt aus den folgenden ADC- und Spannungs-Werten 
      3.665  4.2
      3.296  3.8
      2.916  3.4
      2.543  3.0
      2.072  2.5
  */

    if (adc < 1 || adc > 4095)
      return 0;

    adc = adc / 1000;

    return adc;

    // Device 1
    return 5.814402272E-2 * pow(adc, 4) - 6.614275168E-1 * pow(adc, 3) + 2.785160725 * pow(adc, 2) - 4.077128492 * adc + 3.802638788;

    // Device 2
    // return 5.814402272E-2 * pow(adc, 4) - 6.614275168E-1 * pow(adc, 3) + 2.785160725 * pow(adc, 2) - 4.077128492 * adc + 3.802638788;
  }

  void readSensorValues()
  {
    initVoltage();

    // read LiPo Voltage (max 4.2 Volt)
    adc_reading_42V = readRoundedAdc(ADC1_CHANNEL_7);
    // read VCC Voltage (3.3 Volt)
    adc_reading_33V = readRoundedAdc(ADC1_CHANNEL_6);

    float externalVoltage = calulateVoltageCompensated(adc_reading_42V);
    float vccVoltage = (float)adc_reading_33V * 2 * 2.2 / 4095; // wegen ADC_ATTEN_DB_6

    printf("External Voltage: %f Volt, \tVCC-Voltage: %f Volt)\n", externalVoltage, vccVoltage);

    int waterLevel = gpio_get_level(GPIO_NUM_35);
    printf("Water Level is %s  %i \n", waterLevel == 0 ? "Low" : "High", waterLevel);

    lpp.reset();
    lpp.addDigitalInput(0, waterLevel);
    lpp.addAnalogInput(2, externalVoltage);
    lpp.addAnalogInput(3, vccVoltage);

    // prepareVoltageMessage(voltage_42V);
  }
#ifdef __cplusplus
}
#endif