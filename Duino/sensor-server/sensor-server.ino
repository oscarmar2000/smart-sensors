#include "WebPublisher.hpp"
#include "SensorConfig.hpp"
#include "SensorCtrl.hpp"

const char *ssid = "WIFI_NETWORK_NAME";
const char *password = "SSID_PASSWORD";


auto ms1 = DigitalSensor({1});
auto ms2 = AnalogSensor(AnalogConfig{2, 0, 5.0, 1024});
auto ms3 = DiscreteSensor({3});

SensorCtrl ctrl(ms1, ms2, ms3, DigitalSensor(DigitalConfig{3}));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(1000);

  // Wifi configuration
  ctrl.wifiSetup(ssid, password);

  delay(1000);
  ctrl.begin();

}


void loop() {
  // put your main code here, to run repeatedly:

  ctrl.checkForSubscriber();

  ctrl.update_all();
  delay(1000);
}
