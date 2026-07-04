#pragma once
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace xantrex_custom {

class Xantrex : public Component, public uart::UARTDevice {
 public:
  Xantrex() = default;

  void set_kwhlife_sensor(sensor::Sensor *s) { kwhlife_sensor = s; }
  void set_kwhtoday_sensor(sensor::Sensor *s) { kwhtoday_sensor = s; }
  void set_pin_sensor(sensor::Sensor *s) { pin_sensor = s; }
  void set_pout_sensor(sensor::Sensor *s) { pout_sensor = s; }
  void set_vin_sensor(sensor::Sensor *s) { vin_sensor = s; }
  void set_vout_sensor(sensor::Sensor *s) { vout_sensor = s; }
  void set_iin_sensor(sensor::Sensor *s) { iin_sensor = s; }
  void set_iout_sensor(sensor::Sensor *s) { iout_sensor = s; }
  void set_freq_sensor(sensor::Sensor *s) { freq_sensor = s; }
  void set_time_sensor(sensor::Sensor *s) { time_sensor = s; }
  void set_temp_sensor(sensor::Sensor *s) { temp_sensor = s; }

  void setup() override {
    cycle_start_time = millis() - CYCLE_WAIT;
    command_issued_time = cycle_start_time;
    command_issued = false;
  }

  void loop() override;

 protected:
  bool command_issued = false;
  unsigned long cycle_start_time = 0, command_issued_time = 0;
  static const int RESPONSE_WAIT = 2500;
  static const int CYCLE_WAIT = 5 * 60 * 1000;
  const char *queries[11] = {
    "KWHLIFE?\r", "KWHTODAY?\r", "PIN?\r", "POUT?\r",
    "VIN?\r", "VOUT?\r", "IIN?\r", "IOUT?\r",
    "FREQ?\r", "TIME?\r", "MEASTEMP?\r"
  };
  int queryNum = 0;

  sensor::Sensor *kwhlife_sensor{nullptr};
  sensor::Sensor *kwhtoday_sensor{nullptr};
  sensor::Sensor *pin_sensor{nullptr};
  sensor::Sensor *pout_sensor{nullptr};
  sensor::Sensor *vin_sensor{nullptr};
  sensor::Sensor *vout_sensor{nullptr};
  sensor::Sensor *iin_sensor{nullptr};
  sensor::Sensor *iout_sensor{nullptr};
  sensor::Sensor *freq_sensor{nullptr};
  sensor::Sensor *time_sensor{nullptr};
  sensor::Sensor *temp_sensor{nullptr};

  int readline(char *buffer, int len);
  void stripfarenheit(char *buffer, int len);
};

inline int Xantrex::readline(char *buffer, int len) {
  static int pos = 0;
  int rpos, readch;
  while (available() > 0) {
    readch = read();
    switch (readch) {
      case '\n': break;
      case '\r':
        rpos = pos;
        pos = 0;
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  return -1;
}

inline void Xantrex::stripfarenheit(char *buffer, int len) {
  static int pos = 0;
  for (pos = 0; buffer[pos] != '\0' && pos < len - 1; pos++) {
    switch (buffer[pos]) {
      case 'C':
      case ':':
        buffer[pos] = ' ';
        break;
      case ' ':
        buffer[pos] = 0;
        pos = len - 1;
        break;
    }
  }
}

inline void Xantrex::loop() {
  const int max_line_length = 80;
  static char buffer[max_line_length];

  if (command_issued) {
    if (millis() > command_issued_time + RESPONSE_WAIT) {
      ESP_LOGD("Xantrex", "RESPONSE_WAIT elapsed, available (%d)", available());
      if (readline(buffer, max_line_length) >= 0) {
        float val = atof(buffer);
        switch (queryNum) {
          case 0: if (kwhlife_sensor) kwhlife_sensor->publish_state(val); break;
          case 1: if (kwhtoday_sensor) kwhtoday_sensor->publish_state(val); break;
          case 2: if (pin_sensor) pin_sensor->publish_state(val); break;
          case 3: if (pout_sensor) pout_sensor->publish_state(val); break;
          case 4: if (vin_sensor) vin_sensor->publish_state(val); break;
          case 5: if (vout_sensor) vout_sensor->publish_state(val); break;
          case 6: if (iin_sensor) iin_sensor->publish_state(val); break;
          case 7: if (iout_sensor) iout_sensor->publish_state(val); break;
          case 8: if (freq_sensor) freq_sensor->publish_state(val); break;
          case 9: if (time_sensor) time_sensor->publish_state(val); break;
          case 10:
            stripfarenheit(buffer, max_line_length);
            if (temp_sensor) temp_sensor->publish_state(atof(buffer));
            break;
        }
      }
      queryNum++;
      if (queryNum > 10) queryNum = 0;
      buffer[0] = 0;
      command_issued = false;
    }
  }

  if (millis() > cycle_start_time + CYCLE_WAIT || (queryNum > 0 && !command_issued)) {
    ESP_LOGD("Xantrex", "Issuing command %d", queryNum);
    write_str(queries[queryNum]);
    flush();
    command_issued = true;
    command_issued_time = millis();
    if (queryNum == 0) {
      cycle_start_time = cycle_start_time + CYCLE_WAIT;
    }
  }
}

}  // namespace xantrex_custom
}  // namespace esphome
