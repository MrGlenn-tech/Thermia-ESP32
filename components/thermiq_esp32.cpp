#include "thermiq_esp32.h"

namespace esphome {
namespace thermiq_esp32 {

static const char *const TAG = "thermiq_esp32";
ThermiqComponent *ThermiqComponent::instance = nullptr;

static RegValue REGS[] = {
  {0x00,"outdoor_temp","Outdoor temp","C",0,false},
  {0x01,"indoor_temp","Indoor temp","C",0,false},
  {0x02,"indoor_temp_decimal","Indoor temp decimal","",0,false},
  {0x03,"indoor_target","Indoor target","C",0,false},
  {0x04,"indoor_target_decimal","Indoor target decimal","",0,false},
  {0x05,"supply_line","Supply line","C",0,false},
  {0x06,"return_line","Return line","C",0,false},
  {0x07,"hotwater_temp","Hot water","C",0,false},
  {0x08,"brine_out","Brine out","C",0,false},
  {0x09,"brine_in","Brine in","C",0,false},
  {0x0A,"cooling_temp","Cooling temp","C",0,false},
  {0x0B,"supply_shunt","Supply shunt","C",0,false},
  {0x0C,"current_consumed","Current consumed","A",0,false},
  {0x0D,"aux_status","Aux status","",0,false},
  {0x0E,"supply_target","Supply target","C",0,false},
  {0x0F,"supply_target_shunt","Supply target shunt","C",0,false},
  {0x10,"status_r10","Status r10","",0,false},
  {0x11,"status_r11","Status r11","",0,false},
  {0x12,"pwm_out","PWM out","",0,false},
  {0x13,"alarm_r13","Alarm r13","",0,false},
  {0x14,"alarm_r14","Alarm r14","",0,false},
  {0x15,"demand1_r15","Demand1","",0,false},
  {0x16,"demand2_r16","Demand2","",0,false},
  {0x17,"pressure_pipe","Pressure pipe","C",0,false},
  {0x18,"hotwater_supply","Hotwater supply","C",0,false},
  {0x19,"integral","Integral","",0,false},
  {0x1A,"integral_alimit","Integral A-limit","",0,false},
  {0x1B,"defrost","Defrost","",0,false},
  {0x1C,"min_time_to_start","Min time to start","min",0,false},
  {0x1D,"program_version","Program version","",0,false},
  {0x1E,"flowlinepump_speed","Flowline pump speed","%",0,false},
  {0x1F,"brinepump_speed","Brine pump speed","%",0,false},
  {0x20,"status3_r20","Status3","",0,false},
  {0x32,"indoor_target_control","Indoor target control","C",0,false},
  {0x33,"mode","Mode","",0,false},
  {0x34,"curve","Curve","",0,false},
  {0x35,"curve_min","Curve min","",0,false},
  {0x36,"curve_max","Curve max","",0,false},
  {0x37,"curve_p5","Curve +5","",0,false},
  {0x38,"curve_0","Curve 0","",0,false},
  {0x39,"curve_m5","Curve -5","",0,false},
  {0x3A,"heatstop","Heat stop","C",0,false},
  {0x3B,"reduction","Reduction","C",0,false},
  {0x3C,"room_factor","Room factor","",0,false},
  {0x43,"pressure_pipe_limit","Pressure pipe limit","C",0,false},
  {0x44,"hotwater_start","Hotwater start","C",0,false},
  {0x45,"hotwater_runtime","Hotwater runtime","min",0,false},
  {0x46,"heatpump_runtime","Heatpump runtime","min",0,false},
  {0x49,"integral_limit_a1","Integral limit A1","",0,false},
  {0x4A,"hysteresis_heatpump","Hysteresis heatpump","C",0,false},
  {0x4B,"return_max_limit","Return max limit","C",0,false},
  {0x4C,"min_start_interval","Min start interval","min",0,false},
  {0x4D,"brine_min_limit","Brine min limit","C",0,false},
  {0x4F,"integral_limit_a2","Integral limit A2","",0,false},
  {0x50,"hysteresis_aux","Hysteresis aux","C",0,false},
  {0x51,"max_step_aux","Max step aux","",0,false},
  {0x52,"current_max","Current max","A",0,false},
  {0x53,"shunt_time","Shunt time","s",0,false},
  {0x54,"hotwater_stop","Hotwater stop","C",0,false},
  {0x57,"language","Language","",0,false},
  {0x61,"heatingsystem_type","Heating system type","",0,false},
  {0x62,"options","Options","",0,false},
  {0x63,"logging_time","Logging time","min",0,false},
  {0x68,"runtime_compressor","Runtime compressor","h",0,false},
  {0x6A,"runtime_3kw","Runtime 3kW","h",0,false},
  {0x6C,"runtime_hotwater","Runtime hotwater","h",0,false},
  {0x72,"runtime_6kw","Runtime 6kW","h",0,false}
};

static const uint8_t POLL_REGS[] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
  0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,
  0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
  0x1E,0x1F,0x20,
  0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
  0x3C,0x43,0x44,0x45,0x46,0x49,0x4A,0x4B,0x4C,0x4D,
  0x4F,0x50,0x51,0x52,0x53,0x54,0x57,0x61,0x62,0x63,
  0x68,0x6A,0x6C,0x72
};

static void i2c_receive_trampoline(int len) {
  if (ThermiqComponent::instance != nullptr) ThermiqComponent::instance->on_i2c_receive(len);
}

static void i2c_request_trampoline() {
  if (ThermiqComponent::instance != nullptr) ThermiqComponent::instance->on_i2c_request();
}

static void mqtt_callback_trampoline(char *topic, uint8_t *payload, unsigned int length) {
  if (ThermiqComponent::instance != nullptr) ThermiqComponent::instance->on_mqtt_message(topic, payload, length);
}

void ThermiqComponent::setup() {
  ThermiqComponent::instance = this;
  this->add_log_("Booting Thermia MQTT + ThermIQ ESPHome...");

  Wire.setPins(this->sda_pin_, this->scl_pin_);
  Wire.begin((uint8_t) this->i2c_address_);
  Wire.onReceive(i2c_receive_trampoline);
  Wire.onRequest(i2c_request_trampoline);
  this->add_log_("I2C slave started at address 0x" + this->hx_(this->i2c_address_));

  this->mqtt_.setServer(this->mqtt_host_.c_str(), this->mqtt_port_);
  this->mqtt_.setCallback(mqtt_callback_trampoline);
  this->mqtt_.setBufferSize(4096);
  this->mqtt_.setKeepAlive(60);
  this->mqtt_.setSocketTimeout(5);

  this->queue_poll_sequence_start_();
}

void ThermiqComponent::loop() {
  uint32_t now = millis();

  if (this->mqtt_.connected()) this->mqtt_.loop();

  if (!this->mqtt_.connected() && WiFi.status() == WL_CONNECTED) {
    if (now - this->last_mqtt_try_ > 5000) {
      this->last_mqtt_try_ = now;
      this->reconnect_mqtt_();
    }
  }

  if (now - this->last_poll_ > this->poll_interval_) {
    this->last_poll_ = now;
    this->queue_poll_sequence_start_();
  }

  if (now - this->last_step_ > 350) {
    this->last_step_ = now;
    this->queue_poll_step_();
  }

  if (now - this->last_mqtt_publish_ > this->mqtt_publish_interval_) {
    this->last_mqtt_publish_ = now;
    if (this->mqtt_.connected()) this->publish_state_();
  }
}

void ThermiqComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "ThermIQ MQTT compatibility component");
  ESP_LOGCONFIG(TAG, "  MQTT host: %s:%u", this->mqtt_host_.c_str(), this->mqtt_port_);
  ESP_LOGCONFIG(TAG, "  Base topic: %s", this->base_topic_.c_str());
  ESP_LOGCONFIG(TAG, "  ThermIQ base topic: %s", this->thermiq_base_topic_.c_str());
  ESP_LOGCONFIG(TAG, "  I2C slave: SDA GPIO%u, SCL GPIO%u, address 0x%02X", this->sda_pin_, this->scl_pin_, this->i2c_address_);
}

void ThermiqComponent::add_log_(const String &s) {
  ESP_LOGI(TAG, "%s", s.c_str());
  this->log_buffer_ += s + "\n";
  if (this->log_buffer_.length() > 9000) this->log_buffer_ = this->log_buffer_.substring(this->log_buffer_.length() - 6500);
}

String ThermiqComponent::hx_(uint8_t b) {
  String s = String(b, HEX);
  s.toUpperCase();
  if (s.length() < 2) s = "0" + s;
  return s;
}

String ThermiqComponent::mode_name_(int16_t mode) {
  if (mode == 1) return "Auto";
  if (mode == 2) return "Heat pump";
  if (mode == 3) return "Auxiliary";
  if (mode == 4) return "Hot water";
  return "Unknown";
}

void ThermiqComponent::update_register_(uint8_t reg, int16_t value) {
  for (auto &r : REGS) {
    if (r.reg == reg) {
      r.value = value;
      r.valid = true;
      return;
    }
  }
}

bool ThermiqComponent::get_reg_(uint8_t reg, int16_t &value) {
  for (auto &r : REGS) {
    if (r.reg == reg && r.valid) {
      value = r.value;
      return true;
    }
  }
  return false;
}

String ThermiqComponent::get_reg_json_(uint8_t reg) {
  int16_t value;
  if (this->get_reg_(reg, value)) return String(value);
  return "null";
}

bool ThermiqComponent::extract_json_int_(String msg, String key, int &value) {
  int p = msg.indexOf("\"" + key + "\"");
  if (p < 0) return false;
  int colon = msg.indexOf(":", p);
  if (colon < 0) return false;
  int end = msg.indexOf(",", colon);
  if (end < 0) end = msg.indexOf("}", colon);
  if (end < 0) return false;
  value = msg.substring(colon + 1, end).toInt();
  return true;
}

bool ThermiqComponent::extract_json_float_(String msg, String key, float &value) {
  int p = msg.indexOf("\"" + key + "\"");
  if (p < 0) return false;
  int colon = msg.indexOf(":", p);
  if (colon < 0) return false;
  int end = msg.indexOf(",", colon);
  if (end < 0) end = msg.indexOf("}", colon);
  if (end < 0) return false;
  value = msg.substring(colon + 1, end).toFloat();
  return true;
}

void ThermiqComponent::queue_read_(uint8_t reg) {
  if (this->has_queued_command_ || this->has_queued_write_) return;
  this->queued_register_ = reg;
  this->has_queued_command_ = true;
}

void ThermiqComponent::queue_write_int16_(uint8_t reg, int16_t value) {
  if (this->has_queued_command_ || this->has_queued_write_) {
    this->add_log_("Busy write r" + this->hx_(reg));
    return;
  }
  this->queued_write_reg_ = reg | 0x80;
  this->queued_write_lo_ = (uint8_t)(value & 0xFF);
  this->queued_write_hi_ = (uint8_t)((value >> 8) & 0xFF);
  this->has_queued_write_ = true;
  this->update_register_(reg, value);
  if (reg == 0x32) this->update_register_(0x03, value);
  this->add_log_("WRITE r" + this->hx_(reg) + "=" + String(value));
  this->queue_poll_sequence_start_();
}

void ThermiqComponent::on_i2c_receive(int len) {
  uint8_t buf[8];
  int n = 0;
  while (Wire.available() && n < 8) buf[n++] = Wire.read();

  if (n == 1) {
    uint8_t b = buf[0];
    if (b == 0xFE || b == 0xFD) {
      if (this->has_queued_write_) this->next_response_ = this->queued_write_reg_;
      else if (this->has_queued_command_) {
        this->next_response_ = this->queued_register_;
        this->has_queued_command_ = false;
      } else this->next_response_ = 0xFF;
    }
    return;
  }

  if (n >= 3) {
    uint8_t reg = buf[0];
    int16_t value = (int16_t)((uint16_t)buf[1] | ((uint16_t)buf[2] << 8));
    this->update_register_(reg, value);
  }
}

void ThermiqComponent::on_i2c_request() {
  if (this->has_queued_write_) {
    uint8_t packet[3] = {this->queued_write_reg_, this->queued_write_lo_, this->queued_write_hi_};
    Wire.write(packet, 3);
    this->has_queued_write_ = false;
    this->add_log_("TX WRITE " + this->hx_(packet[0]) + " " + this->hx_(packet[1]) + " " + this->hx_(packet[2]));
  } else {
    Wire.write(this->next_response_);
  }
  this->next_response_ = 0xFF;
}

void ThermiqComponent::queue_poll_sequence_start_() {
  this->poll_index_ = 0;
  this->poll_active_ = true;
  this->add_log_("Poll start");
}

void ThermiqComponent::queue_poll_step_() {
  if (!this->poll_active_) return;
  if (this->has_queued_command_ || this->has_queued_write_) return;
  if (this->poll_index_ >= (int)(sizeof(POLL_REGS) / sizeof(POLL_REGS[0]))) {
    this->poll_active_ = false;
    this->add_log_("Poll done");
    return;
  }
  this->queue_read_(POLL_REGS[this->poll_index_]);
  this->poll_index_++;
}

String ThermiqComponent::state_topic_() { return String(this->base_topic_.c_str()) + "/state"; }
String ThermiqComponent::availability_topic_() { return String(this->base_topic_.c_str()) + "/availability"; }
String ThermiqComponent::set_topic_() { return String(this->base_topic_.c_str()) + "/set"; }
String ThermiqComponent::thermiq_data_topic_() { return String(this->thermiq_base_topic_.c_str()) + "/data"; }
String ThermiqComponent::thermiq_set_topic_() { return String(this->thermiq_base_topic_.c_str()) + "/set"; }
String ThermiqComponent::thermiq_write_topic_() { return String(this->thermiq_base_topic_.c_str()) + "/write"; }

void ThermiqComponent::publish_discovery_sensor_(const char *key, const char *name, const char *unit) {
  String topic = String(this->discovery_prefix_.c_str()) + "/sensor/" + this->device_id_.c_str() + "/" + key + "/config";
  String payload = "{";
  payload += "\"name\":\"" + String(name) + "\",";
  payload += "\"unique_id\":\"" + String(this->device_id_.c_str()) + "_" + String(key) + "\",";
  payload += "\"state_topic\":\"" + this->state_topic_() + "\",";
  payload += "\"availability_topic\":\"" + this->availability_topic_() + "\",";
  payload += "\"value_template\":\"{{ value_json." + String(key) + " }}\",";
  if (String(unit).length() > 0) payload += "\"unit_of_measurement\":\"" + String(unit) + "\",";
  payload += "\"device\":{\"identifiers\":[\"" + String(this->device_id_.c_str()) + "\"],\"name\":\"" + String(this->device_name_.c_str()) + "\",\"manufacturer\":\"DIY\",\"model\":\"Thermia I2C MQTT\"}";
  payload += "}";
  this->mqtt_.publish(topic.c_str(), payload.c_str(), true);
}

void ThermiqComponent::publish_discovery_text_(const char *key, const char *name) {
  String topic = String(this->discovery_prefix_.c_str()) + "/sensor/" + this->device_id_.c_str() + "/" + key + "/config";
  String payload = "{";
  payload += "\"name\":\"" + String(name) + "\",";
  payload += "\"unique_id\":\"" + String(this->device_id_.c_str()) + "_" + String(key) + "\",";
  payload += "\"state_topic\":\"" + this->state_topic_() + "\",";
  payload += "\"availability_topic\":\"" + this->availability_topic_() + "\",";
  payload += "\"value_template\":\"{{ value_json." + String(key) + " }}\",";
  payload += "\"device\":{\"identifiers\":[\"" + String(this->device_id_.c_str()) + "\"],\"name\":\"" + String(this->device_name_.c_str()) + "\",\"manufacturer\":\"DIY\",\"model\":\"Thermia I2C MQTT\"}";
  payload += "}";
  this->mqtt_.publish(topic.c_str(), payload.c_str(), true);
}

void ThermiqComponent::publish_discovery_() {
  for (auto &r : REGS) {
    this->publish_discovery_sensor_(r.key, r.name, r.unit);
    delay(15);
    this->mqtt_.loop();
  }
  this->publish_discovery_text_("mode_name", "Mode name");
  this->publish_discovery_text_("last_update", "Last update");
  this->add_log_("Discovery published");
}

void ThermiqComponent::append_reg_(String &payload, const char *key, uint8_t reg, bool comma) {
  payload += "\"";
  payload += key;
  payload += "\":";
  payload += this->get_reg_json_(reg);
  if (comma) payload += ",";
}

void ThermiqComponent::publish_thermiq_data_() {
  String payload = "{";
  payload += "\"Client_Name\":\"ThermIQ_ESP32\",";
  this->append_reg_(payload, "r00", 0x00); this->append_reg_(payload, "r01", 0x01); this->append_reg_(payload, "r02", 0x02); this->append_reg_(payload, "r03", 0x03);
  this->append_reg_(payload, "r04", 0x04); this->append_reg_(payload, "r05", 0x05); this->append_reg_(payload, "r06", 0x06); this->append_reg_(payload, "r07", 0x07);
  this->append_reg_(payload, "r08", 0x08); this->append_reg_(payload, "r09", 0x09); this->append_reg_(payload, "r0a", 0x0A); this->append_reg_(payload, "r0b", 0x0B);
  this->append_reg_(payload, "r0c", 0x0C); this->append_reg_(payload, "r0d", 0x0D); this->append_reg_(payload, "r0e", 0x0E); this->append_reg_(payload, "r0f", 0x0F);
  this->append_reg_(payload, "r10", 0x10); this->append_reg_(payload, "r11", 0x11); this->append_reg_(payload, "r12", 0x12); this->append_reg_(payload, "r13", 0x13);
  this->append_reg_(payload, "r14", 0x14); this->append_reg_(payload, "r15", 0x15); this->append_reg_(payload, "r16", 0x16); this->append_reg_(payload, "r17", 0x17);
  this->append_reg_(payload, "r18", 0x18); this->append_reg_(payload, "r19", 0x19); this->append_reg_(payload, "r1a", 0x1A); this->append_reg_(payload, "r1b", 0x1B);
  this->append_reg_(payload, "r1c", 0x1C); this->append_reg_(payload, "r1d", 0x1D); this->append_reg_(payload, "r1e", 0x1E); this->append_reg_(payload, "r1f", 0x1F); this->append_reg_(payload, "r20", 0x20);
  this->append_reg_(payload, "r32", 0x32); this->append_reg_(payload, "r33", 0x33); this->append_reg_(payload, "r34", 0x34); this->append_reg_(payload, "r35", 0x35);
  this->append_reg_(payload, "r36", 0x36); this->append_reg_(payload, "r37", 0x37); this->append_reg_(payload, "r38", 0x38); this->append_reg_(payload, "r39", 0x39);
  this->append_reg_(payload, "r3a", 0x3A); this->append_reg_(payload, "r3b", 0x3B); this->append_reg_(payload, "r3c", 0x3C); this->append_reg_(payload, "r43", 0x43);
  this->append_reg_(payload, "r44", 0x44); this->append_reg_(payload, "r45", 0x45); this->append_reg_(payload, "r46", 0x46); this->append_reg_(payload, "r49", 0x49);
  this->append_reg_(payload, "r4a", 0x4A); this->append_reg_(payload, "r4b", 0x4B); this->append_reg_(payload, "r4c", 0x4C); this->append_reg_(payload, "r4d", 0x4D);
  this->append_reg_(payload, "r4f", 0x4F); this->append_reg_(payload, "r50", 0x50); this->append_reg_(payload, "r51", 0x51); this->append_reg_(payload, "r52", 0x52);
  this->append_reg_(payload, "r53", 0x53); this->append_reg_(payload, "r54", 0x54); this->append_reg_(payload, "r57", 0x57); this->append_reg_(payload, "r61", 0x61);
  this->append_reg_(payload, "r62", 0x62); this->append_reg_(payload, "r63", 0x63); this->append_reg_(payload, "r68", 0x68); this->append_reg_(payload, "r6a", 0x6A);
  this->append_reg_(payload, "r6c", 0x6C); this->append_reg_(payload, "r72", 0x72);
  payload += "\"d050\":" + this->get_reg_json_(0x32) + ",";
  payload += "\"d051\":" + this->get_reg_json_(0x33) + ",";
  payload += "\"d052\":" + this->get_reg_json_(0x34) + ",";
  payload += "\"d053\":" + this->get_reg_json_(0x35) + ",";
  payload += "\"d054\":" + this->get_reg_json_(0x36) + ",";
  payload += "\"d058\":" + this->get_reg_json_(0x3A) + ",";
  payload += "\"d068\":" + this->get_reg_json_(0x44) + ",";
  payload += "\"d084\":" + this->get_reg_json_(0x54) + ",";
  if (this->indr_temp_ > -50.0) payload += "\"INDR_T\":" + String(this->indr_temp_, 1) + ",";
  payload += "\"EVU\":" + String(this->evu_state_) + ",";
  payload += "\"vp_read\":\"Ok\",";
  payload += "\"time\":" + String(millis() / 1000);
  payload += "}";
  this->mqtt_.publish(this->thermiq_data_topic_().c_str(), payload.c_str(), false);
}

void ThermiqComponent::publish_state_() {
  int16_t mode = 0;
  this->get_reg_(0x33, mode);
  String payload = "{";
  bool first = true;
  for (auto &r : REGS) {
    if (!first) payload += ",";
    first = false;
    payload += "\"" + String(r.key) + "\":";
    payload += r.valid ? String(r.value) : "null";
  }
  payload += ",\"mode_name\":\"" + this->mode_name_(mode) + "\"";
  payload += ",\"evu\":" + String(this->evu_state_);
  if (this->indr_temp_ > -50.0) payload += ",\"indr_t\":" + String(this->indr_temp_, 1);
  payload += ",\"last_update\":" + String(millis() / 1000);
  payload += "}";
  this->mqtt_.publish(this->state_topic_().c_str(), payload.c_str(), false);
  this->mqtt_.publish(this->availability_topic_().c_str(), "online", true);
  this->publish_thermiq_data_();
}

void ThermiqComponent::on_mqtt_message(char *topic, uint8_t *payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  this->add_log_("MQTT [" + String(topic) + "]: " + msg);
  int v;
  float f;
  if (this->extract_json_float_(msg, "INDR_T", f)) {
    this->indr_temp_ = f;
    this->add_log_("INDR_T received = " + String(this->indr_temp_, 1) + " - stored only");
    this->publish_state_();
    return;
  }
  if (this->extract_json_int_(msg, "d050", v)) {
    if (v >= 10 && v <= 30) {
      this->add_log_("ThermIQ WRITE d050 -> r32 = " + String(v));
      this->queue_write_int16_(0x32, v);
      this->publish_state_();
      return;
    }
  }
  if (this->extract_json_int_(msg, "d051", v)) {
    if (v >= 1 && v <= 4) {
      this->add_log_("ThermIQ WRITE d051 -> r33 = " + String(v));
      this->queue_write_int16_(0x33, v);
      this->publish_state_();
      return;
    }
    if (v == 0) {
      this->add_log_("ThermIQ WRITE d051=0 ignored");
      this->publish_state_();
      return;
    }
  }
  if (this->extract_json_int_(msg, "indoor_target", v)) {
    if (v >= 10 && v <= 30) {
      this->queue_write_int16_(0x32, v);
      this->publish_state_();
      return;
    }
  }
  if (this->extract_json_int_(msg, "indoor_target_step", v)) {
    int16_t current;
    if (this->get_reg_(0x32, current) || this->get_reg_(0x03, current)) {
      int target = current + v;
      if (target >= 10 && target <= 30) {
        this->queue_write_int16_(0x32, target);
        this->publish_state_();
        return;
      }
    }
  }
  if (this->extract_json_int_(msg, "mode", v)) {
    if (v >= 1 && v <= 4) {
      this->queue_write_int16_(0x33, v);
      this->publish_state_();
      return;
    }
  }
  if (this->extract_json_int_(msg, "EVU", v)) {
    this->evu_state_ = v ? 1 : 0;
    this->add_log_("EVU state set to " + String(this->evu_state_) + " - pump block not mapped yet");
    this->publish_state_();
    return;
  }
}

void ThermiqComponent::reconnect_mqtt_() {
  if (this->mqtt_.connected()) return;
  String client_id = String(this->device_id_.c_str()) + "_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  bool connected;
  if (this->mqtt_username_.empty()) {
    connected = this->mqtt_.connect(client_id.c_str(), this->availability_topic_().c_str(), 0, true, "offline");
  } else {
    connected = this->mqtt_.connect(client_id.c_str(), this->mqtt_username_.c_str(), this->mqtt_password_.c_str(), this->availability_topic_().c_str(), 0, true, "offline");
  }
  if (connected) {
    this->add_log_("MQTT connected");
    this->mqtt_.publish(this->availability_topic_().c_str(), "online", true);
    this->mqtt_.subscribe(this->set_topic_().c_str());
    this->mqtt_.subscribe(this->thermiq_set_topic_().c_str());
    this->mqtt_.subscribe(this->thermiq_write_topic_().c_str());
    this->publish_discovery_();
    this->publish_state_();
  } else {
    this->add_log_("MQTT failed rc=" + String(this->mqtt_.state()));
  }
}

}  // namespace thermiq_esp32
}  // namespace esphome
