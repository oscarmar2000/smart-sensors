#pragma once

#include "Configurator.hpp"
#include "SensorImpl.hpp"


struct AnalogConfig : Config
{
  float min{0};
  float max{5.0};
  int   resol{1024};
};

//
// analog Port configurator/handler, based on DigConfig info
//
class AnalogPort : public Configurator<AnalogPort, AnalogConfig>
{
    int m_port{};
    float m_step{};
    AnalogConfig m_cfg;

  public:
    using InputType = float;

    //constexpr
    void do_config(const AnalogConfig cfg)
    {
      m_cfg = cfg;
      m_port = cfg.index;
      m_step = (cfg.max - cfg.min)/(float)cfg.resol;

      // pinMode
    }

    InputType read() {
      return static_cast<float>(analogRead(m_port)) * m_step + m_cfg.min;
    }

};

//
// Analog data processing class
//
class DefAnalogProc
{
  public:
    AnalogPort::InputType calc(AnalogPort::InputType in) {
      return in;
    }
};



class AnalogSensor : public SensorImpl<AnalogSensor, float,
  AnalogConfig, AnalogPort, DefAnalogProc>
{
  public:
    AnalogSensor(AnalogConfig dcfg) : SensorImpl(dcfg) {}
};
