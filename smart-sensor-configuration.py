import os
import json
from pathlib import Path
import sys

with open(sys.argv[1], 'r') as my_config:
    config = my_config.read()

#print(config)

configDef = '''
{
    "project": {
        "network": {
            "wifi": {
                "ssid":"SSID_NAME",
                "password":"PASSWORD"
            }
        }
        "boards":[
            {
                "name": "room1",
                "board_type": "esp32:esp32:lolin_s2_mini",
                "sensors":[
                    {
                        "name": "temp1",
                        "type": "analog",
                        "config": {
                            "pin": 1,
                            "min": 0,
                            "max": 5.0,
                            "resol": 1024
                        }
                    }
                ]
            }
        ],
    }
}

'''

req_sn_types = []


def headers():
    return '''
// -*-c++-*-
#include "smart-sensors/WebPublisher.hpp"
#include "smart-sensors/SensorConfig.hpp"
#include "smart-sensors/SensorCtrl.hpp"

'''

def wifi_params(ssid, pw):
    return '''
const char *ssid = "{0}";
const char *password = "{1}";

'''.format(ssid, pw)

def add_analog_sensor(idx, config):
    l0 = "auto cfg{0} = AnalogConfig{{{1},{2},{3},{4}}};\n".format(idx, config['pin'], config['min'], config['max'], config['resol'])
    l1 = "auto sensor{0} = AnalogSensor(cfg{0});\n\n".format(idx)
    return l0 + l1

def add_digital_sensor(idx, config):
    l0 = "auto cfg{0} = DigitalConfig{{{1}}};\n".format(idx, config['pin'])
    l1 = "auto sensor{0} = DigitalSensor(cfg{0});\n\n".format(idx)
    return l0 + l1

def add_control(cnt):
    ret = "SensorCtrl ctrl("
    for i in range(cnt):
        ret = ret + "sensor{0},".format(i)
    ret = ret[:-1] + ");\n"
    return ret

def body():
    return '''
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

'''

data = json.loads(config)

# print(data['project']['boards'])

proj_path = './project/'
if not os.path.isdir(proj_path):
    os.makedirs(proj_path)

for board in data['project']['boards']:
    board_path = os.path.join(proj_path, board['name'])
    if not os.path.isdir(board_path):
        os.makedirs(board_path)
    os.chdir(board_path)
    Path('smart-sensors').symlink_to( '../../Duino/smart-sensors' )
    with open(board['name'] + '.ino', 'w') as duino_file:
        duino_file.write(headers())
        wifi = data['project']['network']['wifi']
        duino_file.write(wifi_params(wifi['ssid'],
                                     wifi['password']))
        cnt = 0
        for sensor in board['sensors']:
            if not req_sn_types.__contains__(sensor['type']):
                req_sn_types.append(sensor['type'])
            if sensor['type'] == 'analog':
                duino_file.write(add_analog_sensor(cnt, sensor['config']))
                cnt += 1
            if sensor['type'] == 'digital':
                duino_file.write(add_digital_sensor(cnt, sensor['config']))
                cnt += 1

        duino_file.write(add_control(cnt))
        duino_file.write(body())
    os.chdir('../..')
    
print(req_sn_types)


