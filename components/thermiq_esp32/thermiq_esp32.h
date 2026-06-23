#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

namespace esphome {
namespace thermiq_esp32 {

struct RegValue {
  uint8_t reg;
  const char *key;
  const char *name;
  const char *unit;
  int16_t value;
  bool valid;
};

class ThermiqComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_mqtt_host(const std::string &v) { mqtt_host_ = v; }
  void set_mqtt_port(uint16_t v) { mqtt_port_ = v; }
  void set_mqtt_username(const std::string &v) { mqtt_username_ = v; }
  void set_mqtt_password(const std::string &v) { mqtt_password_ = v; }
  void set_device_id(const std::string &v) { device_id_ = v; }
  void set_device_name(const std::string &v) { device_name_ = v; }
  void set_discovery_prefix(const std::string &v) { discovery_prefix_ = v; }
  void set_base_topic(const std::string &v) { base_topic_ = v; }
  void set_thermiq_base_topic(const std::string &v) { thermiq_base_topic_ = v; }
  void set_sda_pin(uint8_t v) { sda_pin_ = v; }
  void set_scl_pin(uint8_t v) { scl_pin_ = v; }
  void set_i2c_address(uint8_t v) { i2c_address_ = v; }
  void set_poll_interval_ms(uint32_t v) { poll_interval_ = v; }
  void set_publish_interval_ms(uint32_t v) { mqtt_publish_interval_ = v; }

  void on_i2c_receive(int len);
  void on_i2c_request();
  void on_mqtt_message(char *topic, uint8_t *payload, unsigned int length);

  static ThermiqComponent *instance;

 protected:
  void add_log_(const String &s);
  String hx_(uint8_t b);
  String mode_name_(int16_t mode);
  void update_register_(uint8_t reg, int16_t value);
  bool get_reg_(uint8_t reg, int16_t &value);
  String get_reg_json_(uint8_t reg);
  bool extract_json_int_(String msg, String key, int &value);
  bool extract_json_float_(String msg, String key, float &value);
  void queue_read_(uint8_t reg);
  void queue_poll_sequence_start_();
  void queue_poll_step_();
  void queue_write_int16_(uint8_t reg, int16_t value);

  String state_topic_();
  String availability_topic_();
  String set_topic_();
  String thermiq_data_topic_();
  String thermiq_set_topic_();
  String thermiq_write_topic_();

  void publish_discovery_sensor_(const char *key, const char *name, const char *unit);
  void publish_discovery_text_(const char *key, const char *name);
  void publish_discovery_();
  void append_reg_(String &payload, const char *key, uint8_t reg, bool comma = true);
  void publish_thermiq_data_();
  void publish_state_();
  void reconnect_mqtt_();

  std::string mqtt_host_{""};
  uint16_t mqtt_port_{1883};
  std::string mqtt_username_{""};
  std::string mqtt_password_{""};
  std::string device_id_{"thermia_esp32"};
  std::string device_name_{"Thermia ESP32"};
  std::string discovery_prefix_{"homeassistant"};
  std::string base_topic_{"thermia/esp32"};
  std::string thermiq_base_topic_{"ThermIQ/ThermIQ-mqtt"};

  uint8_t sda_pin_{21};
  uint8_t scl_pin_{22};
  uint8_t i2c_address_{0x2E};

  WiFiClient wifi_client_;
  PubSubClient mqtt_{wifi_client_};

  volatile uint8_t next_response_{0xFF};
  volatile bool has_queued_command_{false};
  volatile bool has_queued_write_{false};
  volatile uint8_t queued_register_{0xFF};
  volatile uint8_t queued_write_reg_{0xFF};
  volatile uint8_t queued_write_lo_{0x00};
  volatile uint8_t queued_write_hi_{0x00};

  uint32_t last_poll_{0};
  uint32_t last_mqtt_publish_{0};
  uint32_t last_step_{0};
  uint32_t last_mqtt_try_{0};
  uint32_t poll_interval_{300000};
  uint32_t mqtt_publish_interval_{30000};

  int poll_index_{0};
  bool poll_active_{false};
  String log_buffer_;
  int evu_state_{0};
  float indr_temp_{-100.0};
};

}  // namespace thermiq_esp32
}  // namespace esphome
