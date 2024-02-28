#pragma once

#include "SensorConfig.hpp"

//
// Sensor CRTP base class
//
template<typename Derived, typename Tp, typename Config,
         typename HwPort, typename Proc>
class SensorImpl
{
    HwPort m_port{};
    Proc   m_proc{};
    const Config m_cfg;

  public:

    typedef Tp DataTp;

    static constexpr int TpID = snsrTp_val<Derived>;

    constexpr SensorImpl(const Config cfg)
      : m_cfg(cfg)
    {
    }

    void printInfo()
    {
      //TODO
    }

    void config()
    {
      m_port.config(m_cfg);
    }

    Tp get_value()
    {
      return m_proc.calc(m_port.read());
    }

};
