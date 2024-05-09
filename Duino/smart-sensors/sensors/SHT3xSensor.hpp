#pragma once

#include "../Configurator.hpp"
#include "../SensorImpl.hpp"
#include "Wire.h"
#include "Adafruit_SHT31.h"

struct SHT3xConfig : Config
{
    enum class SelValue
    {
        TEMPERATURE,
        HUMIDITY,
    };

    SelValue selValue;
    bool initWire{false};
    int i2cSDA{};
    int i2cSCL{};
  // float min{0};
  // float max{5.0};
  // int   resol{1024};
};

//
// analog Port configurator/handler, based on DigConfig info
//
class SHT3xPort : public Configurator<SHT3xPort, SHT3xConfig>
{
    int m_addr{};
    float m_step{};
    SHT3xConfig m_cfg;
    Adafruit_SHT31 m_sht31 = Adafruit_SHT31();

  public:
    using InputType = float;

    //constexpr
    void do_config(const SHT3xConfig cfg)
    {
      m_cfg  = cfg;
      m_addr = cfg.index;
//      m_step = (cfg.max - cfg.min)/(float)cfg.resol;

      if (m_cfg.initWire)
      {
          Wire.begin(m_cfg.i2cSDA, m_cfg.i2cSCL);
      }

      if (!m_sht31.begin(m_addr))
      {
          Serial.println("sht sensor init failed");
      }
      else
      {
          Serial.println("sht sensor init passed");
          float r = m_sht31.readTemperature();
          Serial.print("Temp *C = "); Serial.print(r); Serial.print("\t\t");
      }
    }

    InputType read() {
        if (m_cfg.selValue == SHT3xConfig::SelValue::TEMPERATURE)
        {
             float r = m_sht31.readTemperature();
             //float r = 33;
            return !isnan(r) ? r : -1;
        }
        return m_sht31.readHumidity(); // analogRead(m_port);
    }

};

//
// SHT3x data processing class
//
class DefSHT3xProc
{
  public:
    SHT3xPort::InputType calc(SHT3xPort::InputType in) {
      return in;
    }
};

// struct SHT3xData
// {
//     float t;
//     float h;
// };

class SHT3xSensor : public SensorImpl<SHT3xSensor, float,
  SHT3xConfig, SHT3xPort, DefSHT3xProc>
{
  public:
    SHT3xSensor(SHT3xConfig dcfg) : SensorImpl(dcfg) {}
};
