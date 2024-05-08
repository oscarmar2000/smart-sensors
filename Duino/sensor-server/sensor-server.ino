// -*-c++-*-

#include "smart-sensors/WebPublisher.hpp"
#include "smart-sensors/SensorConfig.hpp"
#include "smart-sensors/SensorCtrl.hpp"

#include "smart-sensors/SHT3xSensor.hpp"

const char *ssid = "WIFI_NETWORK_NAME";
const char *password = "SSID_PASSWORD";

#define I2C_SDA 8
#define I2C_SCL 9

auto cfg1 = DigitalConfig{1};
auto ms1  = DigitalSensor(cfg1);
auto cfg2 = AnalogConfig{2, 0, 5.0, 1024};
auto ms2  = AnalogSensor(cfg2);
auto cfg3 = AnalogConfig{3};
auto ms3  = DiscreteSensor(cfg3);

auto cfg4 = SHT3xConfig{0x44, SHT3xConfig::SelValue::TEMPERATURE, true, I2C_SDA, I2C_SCL};
auto ms4  = SHT3xSensor(cfg4);

auto ms5 = SHT3xSensor({0x44, SHT3xConfig::SelValue::HUMIDITY, false});

SensorCtrl ctrl(ms1, ms2, ms3, ms4, ms5);

void setup() {
  Serial.begin(115200);

  delay(1000);

  // Wifi configuration
  ctrl.wifiSetup(ssid, password);

  delay(1000);
  ctrl.begin();

}


void loop() {
  ctrl.checkForSubscriber();

  ctrl.update_all();
  delay(1000);
}
