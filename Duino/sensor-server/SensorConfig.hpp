#pragma once

#include <variant>

class DigitalSensor;
class AnalogSensor;
class DiscreteSensor;

typedef std::variant<DigitalSensor, AnalogSensor, DiscreteSensor>    vSensor;
typedef std::variant<DigitalSensor*, AnalogSensor*, DiscreteSensor*> vSensor_ptr;


//
// helper template class to obtain sensor type index
//
template<typename T>
constexpr int SnsrTp()
{
  constexpr T *v = nullptr;
  constexpr vSensor_ptr t{v};

  return t.index();
}

template<typename T>
constexpr int snsrTp_val = SnsrTp<T>();
