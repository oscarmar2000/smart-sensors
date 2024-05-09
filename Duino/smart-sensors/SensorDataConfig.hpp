#pragma once

#include "sensors/DigitalSensor.hpp"
#include "sensors/AnalogSensor.hpp"
#include "sensors/DiscreteSensor.hpp"
#include "sensors/SHT3xSensor.hpp"


typedef std::variant<DigitalSensor::DataTp,
                     AnalogSensor::DataTp,
                     DiscreteSensor::DataTp>    vSensorData;
