#pragma once

#include "AnalogSensor.hpp"

/* struct DiscreteConfig : Config
{
  int min;
  int max;
}; */

// for now using analog config for discrete value
using DiscreteConfig = AnalogConfig;

//
// Dicrete data processing class
//
class DefDiscreteProc
{
  public:
    // this could be replaced by a non-linear conversion
    // or some filtering
    int calc(AnalogPort::InputType in) {
      return static_cast<int>(in); // just cast to int
    }
};


class DiscreteSensor : public SensorImpl<DiscreteSensor, int,
  AnalogConfig, AnalogPort,
  DefDiscreteProc>
{
  public:
    DiscreteSensor(DiscreteConfig dcfg) : SensorImpl(dcfg) {}
};
