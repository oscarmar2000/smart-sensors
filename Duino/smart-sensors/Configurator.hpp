#pragma once

//
// base config struct
//
struct Config
{
  int index;
};


//
// Configurator base class
//
template<typename Deriv, typename Cfg>
class Configurator
{
  public:
    //constexpr
    void config(const Cfg cfg)
    {
      static_cast<Deriv*>(this)->do_config(cfg);
    }
};
