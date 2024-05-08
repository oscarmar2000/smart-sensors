#pragma once

#include <array>
#include "WebPublisher.hpp"

template<typename ... S>
class SensorCtrl : public WebPublisher
{
    std::array<Configs::vSensor, sizeof...(S)> m_sensors;

  public:

    SensorCtrl(S ... s)
      : m_sensors( {std::move(s) ...} )
    {

      /// print all sensor info
      for (auto sn : m_sensors)
      {
        std::visit([](auto && r) {
          r.printInfo();
        }, sn);
      }
    }

    void begin()
    {
        config_all();
        WebPublisher::begin();
    }

    void config_all()
    {
      /// config all sensors
      for (auto &sn : m_sensors)
      {
        std::visit([](auto & r) {
          r.config();
        }, sn);
      }
    }

    void update_all() {

      auto idx = 0;
      for (auto &sn_var : m_sensors)
      {
        std::visit([this, idx](auto & sn) {

          using SnType = typename std::remove_reference<decltype(sn)>::type;

          SensorDataUpdate sdu{idx, sn.TpID,
                               static_cast<typename SnType::DataTp>(sn.get_value()), "Sns"};

          notify(sdu);

        }, sn_var);

        idx++;
      }
    }
};
