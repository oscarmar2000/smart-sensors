#pragma once 


template<typename ... S>
class SensorCtrl : public WebPublisher
{
    std::array<vSensor, sizeof...(S)> m_sensors;

  public:

    SensorCtrl(S...s)
      : m_sensors( {
      std::move(s) ...
    })
    {
      /// config all sensors
      for (auto sn : m_sensors)
      {
        std::visit([](auto && r) {
          r.config();
        }, sn);

      }

      /// print all sensor info
      for (auto sn : m_sensors)
      {
        std::visit([](auto && r) {
          r.printInfo();
        }, sn);
      }

      //m_sensors.push_back(std::make_unique<sensor>(2));
    }

    void update_all() {

      auto idx = 0;
      for (auto sn : m_sensors)
      {
        std::visit([this, idx](auto && r) {

          using TP = typename std::remove_reference<decltype(r)>::type;

          SensorDataUpdate sdu{idx, r.TpID,
                               static_cast<typename TP::DataTp>(r.get_value()), "Sx"};

          notify(sdu);

        }, sn);

        idx++;
      }
    }
};
