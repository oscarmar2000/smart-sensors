#pragma once

#include "Configurator.hpp"
#include "SensorImpl.hpp"


//
// specialized config struct for digital sensor
//  TODOs:
//     - debounce
//     - threshold
//     - speed
//
struct DigitalConfig : Config
{
};


//
// digital Port configurator/handler, based on DigitalConfig info
//
class DigitalPort : public Configurator<DigitalPort, DigitalConfig>
{
    int m_port{};

  public:
    using InputType = bool;

    //constexpr
    void do_config(const DigitalConfig cfg) {
      m_port = cfg.index;
      pinMode(m_port, INPUT);
    }

    InputType read() {
      return (digitalRead(m_port) == HIGH);
    }

};

//
// Optional Processing/filtering class
//
class DefDigitalProc
{
  public:
    DigitalPort::InputType calc(DigitalPort::InputType in) {
      return !in;
    }
};

class DigitalSensor : public SensorImpl<DigitalSensor, DigitalPort::InputType,
  DigitalConfig, DigitalPort,
  DefDigitalProc>
{
  public:
    constexpr DigitalSensor(const DigitalConfig dcfg = DigitalConfig{0}) : SensorImpl(dcfg) {}
};
