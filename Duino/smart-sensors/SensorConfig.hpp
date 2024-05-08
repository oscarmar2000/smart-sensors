#pragma once

#include <variant>

class DigitalSensor;
class AnalogSensor;
class DiscreteSensor;
class SHT3xSensor;

template<typename ... SENSORS>
class BaseConfigs
{

  using vSensor_ptr = std::variant<SENSORS *...>;

  //
  // helper template class to obtain sensor type index
  //
  template <typename T>
  static constexpr int SnsrTp()
  {
    constexpr T *v = nullptr;
    constexpr vSensor_ptr t{v};

    return t.index();
  }

public:
  template <typename T>
  static constexpr int snsrTp_val = SnsrTp<T>();

  using vSensor = std::variant<SENSORS...>;
};

using Configs = BaseConfigs<DigitalSensor, AnalogSensor, DiscreteSensor, SHT3xSensor>;
