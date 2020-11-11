#include <driver/adc.h>
#include <math.h>
#include "esp32-lora-board-pins.h"
#include "CayenneLPP.h"
#include "voltage.h"
#include "sleep-wakeup.h"

#ifdef __cplusplus
extern "C"
{
#endif

  int number_round = 100;
  int adc_reading_42V = 0;
  int adc_reading_33V = 0;

  // WaterLevel Harry: Use GPIO_NUM_35=U_EXT_MEASURE as digital input. So it cannot be used for adc operations
#define USE_GPIO35_DIGITAL_IN

  CayenneLPP lpp(20);

/*   Polynom bestimmt aus den folgenden ADC- und Spannungs-Werten 
      3.665  4.2
      3.296  3.8
      2.916  3.4
      2.543  3.0
      2.072  2.5

      y = 5.814402272·10-2 x4 - 6.614275168·10-1 x3 + 2.785160725 x2 - 4.077128492 x + 3.802638788
 */
  polynom_coeffients_t module1_42Volt = {+3.802638788, -4.077128492, +2.785160725, -6.614275168E-1, 5.814402272E-2};
  polynom_coeffients_t module2_33Volt = {-9.72728919, +12.975906, -5.48479196, +1.095972506, -0.0816047888};

  void initVoltage()
  {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_6); // VCC Voltage

#ifndef USE_GPIO35_DIGITAL_IN
    // WaterLevel Harry: Use GPIO_NUM_35=U_EXT_MEASURE as digital input. So it cannot be used for adc operations
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_6); // external Voltage
#endif
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

  double calulateVoltageCompensated(double adc, polynom_coeffients_t coeff)
  {
    /* 
    google search term: "polynominterpolation online"
    Calulate ploynom with 
    http://www.xuru.org/rt/PR.asp
    http://www.jaik.de/js/Interpolation.htm
    https://valdivia.staff.jade-hs.de/interpol.html
  */

    if (adc < 1 || adc > 4095)
      return 0;

    adc = adc / 1000;

    return coeff.c4 * pow(adc, 4) + coeff.c3 * pow(adc, 3) + coeff.c2 * pow(adc, 2) + coeff.c1 * adc + coeff.c0;
  }

  water_level_t getWaterLevel()
  {
    return gpio_get_level(GPIO_NUM_35) == 0 ? LOW : HIGH;
  }

  void readSensorValues()
  {
    initVoltage();

    // read VCC Voltage (3.3 Volt)
    adc_reading_33V = readRoundedAdc(ADC1_CHANNEL_6);
    float vccVoltage =  calulateVoltageCompensated(adc_reading_33V, module2_33Volt);    
    // float vccVoltage = (float)adc_reading_33V * 2 * 2.2 / 4095; // wegen ADC_ATTEN_DB_6

    printf("VCC-Voltage: %f Volt)\n", vccVoltage);

    lpp.reset();
#ifdef USE_GPIO35_DIGITAL_IN
    // WaterLevel Harry: Use GPIO_NUM_35=U_EXT_MEASURE as digital input. So it cannot be used for adc operations
    // read LiPo Voltage (max 4.2 Volt)
    water_level_t waterLevel = getWaterLevel();
    printf("Water Level is %s  %i \n", waterLevel == HIGH ? "High" : "LOW", waterLevel);
    lpp.addDigitalInput(0, waterLevel);
    lpp.addAnalogInput(1, vccVoltage);
#else
  adc_reading_42V = readRoundedAdc(ADC1_CHANNEL_7);
  float externalVoltage = calulateVoltageCompensated(adc_reading_42V);
  printf("External Voltage: %f Volt, \n", externalVoltage);
  lpp.addAnalogInput(0, vccVoltage);
  lpp.addAnalogInput(1, externalVoltage);
#endif
  }

#ifdef __cplusplus
}
#endif