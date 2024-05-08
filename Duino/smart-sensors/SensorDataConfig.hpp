#pragma once

#include "DigitalSensor.hpp"
#include "AnalogSensor.hpp"
#include "DiscreteSensor.hpp"
#include "SHT3xSensor.hpp"


typedef std::variant<DigitalSensor::DataTp,
                     AnalogSensor::DataTp,
                     DiscreteSensor::DataTp>    vSensorData;
