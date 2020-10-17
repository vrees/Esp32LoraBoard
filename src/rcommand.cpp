#include "globals.h"
#include "esp_log.h"
#include "rcommand.h"
#include "payload.h"

// Local logging tag
static const char TAG[] = __FILE__;

// set of functions that can be triggered by remote commands
void set_reset(uint8_t val[])
{
  switch (val[0])
  {
  case 0: // restart device with cold start (clear RTC saved variables)
    ESP_LOGI(TAG, "Remote command: restart device cold");
    do_reset(false);
    break;
  case 4: // restart device with warm start (keep RTC saved variables)
    ESP_LOGI(TAG, "Remote command: restart device warm");
    do_reset(true);
    break;
  case 9: // reset and ask for software update via Wifi OTA
    ESP_LOGI(TAG, "Remote command: software update via Wifi");
#if (USE_OTA)
    RTC_runmode = RUNMODE_UPDATE;
#endif // USE_OTA
    break;

  default:
    ESP_LOGW(TAG, "Remote command: reset called with invalid parameter(s)");
  }
}

void set_sendcycle(uint8_t val[])
{
  cfg.sendcycle = val[0];
  // update send cycle interrupt [seconds
  // sendcycler.attach(cfg.sendcycle * 2, sendcycle);
  ESP_LOGI(TAG, "Remote command: set send cycle to %d seconds",
           cfg.sendcycle * 2);
}

void set_display(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: set screen to %s", val[0] ? "on" : "off");
  cfg.screenon = val[0] ? 1 : 0;
}

void set_lorapower(uint8_t val[])
{
  cfg.txpower = val[0];
  ESP_LOGI(TAG, "Remote command: set LoRa TXPOWER to %d", cfg.txpower);
  // LMIC_setDrTxpow(assertDR(cfg.loradr), cfg.txpower);
};

void get_config(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get device configuration");
  payload.reset();
  payload.addConfig(cfg);
  SendPayload(CONFIGPORT, prio_high);
  
};

void get_status(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get device status");
  payload.reset();
  payload.addStatus(read_voltage(), uptime() / 1000, temperatureRead(),
                    getFreeRAM(), rtc_get_reset_reason(0),
                    rtc_get_reset_reason(1));
  SendPayload(STATUSPORT, prio_high);
};

void get_gps(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get gps status");
#if (HAS_GPS)
  gpsStatus_t gps_status;
  gps_storelocation(&gps_status);
  payload.reset();
  payload.addGPS(gps_status);
  SendPayload(GPSPORT, prio_high);
#else
  ESP_LOGW(TAG, "GPS function not supported");
#endif
};

void get_bme(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get bme680 sensor data");
#if (HAS_BME)
  payload.reset();
  payload.addBME(bme_status);
  SendPayload(BMEPORT, prio_high);
#else
  ESP_LOGW(TAG, "BME sensor not supported");
#endif
};

void get_batt(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get battery voltage");
#if (defined BAT_MEASURE_ADC || defined HAS_PMU)
  payload.reset();
  payload.addVoltage(read_voltage());
  SendPayload(BATTPORT, prio_normal);
#else
  ESP_LOGW(TAG, "Battery voltage not supported");
#endif
};

void get_time(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: get time");
  payload.reset();
  payload.addTime(now());
  payload.addByte(timeStatus() << 4 | timeSource);
  SendPayload(TIMEPORT, prio_high);
};

void set_time(uint8_t val[])
{
  ESP_LOGI(TAG, "Timesync requested by timeserver");
  timeSync();
};

void set_flush(uint8_t val[])
{
  ESP_LOGI(TAG, "Remote command: flush");
  // does nothing
  // used to open receive window on LoRaWAN class a nodes
};

// assign previously defined functions to set of numeric remote commands
// format: opcode, function, #bytes params,
// flag (true = do make settings persistent / false = don't)
//
static const cmd_t table[] = {
    {0x01, set_rssi, 1, true}, {0x02, set_countmode, 1, true}, {0x03, set_gps, 1, true}, {0x04, set_display, 1, true}, {0x05, set_loradr, 1, true}, {0x06, set_lorapower, 1, true}, {0x07, set_loraadr, 1, true}, {0x08, set_screensaver, 1, true}, {0x09, set_reset, 1, false}, {0x0a, set_sendcycle, 1, true}, {0x0b, set_wifichancycle, 1, true}, {0x0c, set_blescantime, 1, true}, {0x0d, set_vendorfilter, 1, false}, {0x0e, set_blescan, 1, true}, {0x0f, set_wifiant, 1, true}, {0x10, set_rgblum, 1, true}, {0x11, set_monitor, 1, true}, {0x12, set_beacon, 7, false}, {0x13, set_sensor, 2, true}, {0x14, set_payloadmask, 1, true}, {0x15, set_bme, 1, true}, {0x16, set_batt, 1, true}, {0x17, set_wifiscan, 1, true}, {0x80, get_config, 0, false}, {0x81, get_status, 0, false}, {0x83, get_batt, 0, false}, {0x84, get_gps, 0, false}, {0x85, get_bme, 0, false}, {0x86, get_time, 0, false}, {0x87, set_time, 0, false}, {0x99, set_flush, 0, false}};

static const uint8_t cmdtablesize =
    sizeof(table) / sizeof(table[0]); // number of commands in command table

// check and execute remote command
void rcommand(const uint8_t cmd[], const uint8_t cmdlength)
{

  if (cmdlength == 0)
    return;

  uint8_t foundcmd[cmdlength], cursor = 0;
  bool storeflag = false;

  while (cursor < cmdlength)
  {

    int i = cmdtablesize;
    while (i--)
    {
      if (cmd[cursor] == table[i].opcode)
      {           // lookup command in opcode table
        cursor++; // strip 1 byte opcode
        if ((cursor + table[i].params) <= cmdlength)
        {
          memmove(foundcmd, cmd + cursor,
                  table[i].params); // strip opcode from cmd array
          cursor += table[i].params;
          if (table[i].store) // ceck if function needs to store configuration
            storeflag = true;
          table[i].func(
              foundcmd); // execute assigned function with given parameters
        }
        else
          ESP_LOGI(
              TAG,
              "Remote command x%02X called with missing parameter(s), skipped",
              table[i].opcode);
        break; // command found -> exit table lookup loop
      }        // end of command validation
    }          // end of command table lookup loop
    if (i < 0)
    { // command not found -> exit parser
      ESP_LOGI(TAG, "Unknown remote command x%02X, ignored", cmd[cursor]);
      break;
    }
  } // command parsing loop

  if (storeflag)
    saveConfig();
} // rcommand()
