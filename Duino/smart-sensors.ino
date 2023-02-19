#include <variant>
#include <array>
#include <vector>
#include <memory>

#include <WiFi.h>
#include <ArduinoJson.h>


const char *ssid = "WIFI_NETWORK_NAME";
const char *password = "SSID_PASSWORD";


//
// base config struct
//
struct Config
{
  int index;
};

//
// specialized config struct for digital sensor
//  TODOs:
//     - debounce
//     - threshold
//     - speed
//
struct DigConfig : Config
{
};

//
// Configurator base class
//
template<typename Deriv, typename Cfg>
class Configurator
{
  public:
    //constexpr
    void config(Cfg cfg)
    {
      static_cast<Deriv*>(this)->do_config(cfg);
    }
};

//
// digital Port configurator/handler, based on DigConfig info
//
class DigPort : public Configurator<DigPort, DigConfig>
{
    int m_port{};

  public:
    typedef bool InputType;

    //constexpr
    void do_config(DigConfig cfg) {
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
class DefDigProc
{
  public:
    DigPort::InputType calc(DigPort::InputType in) {
      return !in;
    }
};

//
// analog Port configurator/handler, based on DigConfig info
//
class AlgPort : public Configurator<AlgPort, DigConfig>
{
    int m_port{};

  public:
    typedef float InputType;

    //constexpr
    void do_config(DigConfig cfg) {
      m_port = cfg.index;
    }

    InputType read() {
      return (1.23);
    }

};

//
// Analog data processing class
//
class DefAlgProc
{
  public:
    AlgPort::InputType calc(AlgPort::InputType in) {
      return in * 1.5;
    }
};


///
class DigSensor;
class AlgSensor;
class DscSensor;

typedef std::variant<DigSensor, AlgSensor, DscSensor>    vSensor;
typedef std::variant<DigSensor*, AlgSensor*, DscSensor*> vSensor_ptr;

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
///

//
// Sensor CRTP base class
//
template<typename Derived, typename Tp, typename Config,
         typename HwPort, typename Proc>
class SensorTmpl// : public Sensor
{
    HwPort m_port{};
    Proc   m_proc;
    Config m_cfg;

  public:

    typedef Tp DataTp;

    static constexpr int TpID = snsrTp_val<Derived>;

    constexpr SensorTmpl(Config cfg)
      : m_cfg(cfg)
    {

    }

    void printInfo()
    {
    }

    void config()// override
    {
      m_port.config(m_cfg);
    }

    Tp get_value() //override
    {
      return m_proc.calc(m_port.read());
    }

};


class DigSensor : public SensorTmpl<DigSensor, DigPort::InputType,
  DigConfig, DigPort,
  DefDigProc>
{
  public:
    constexpr DigSensor(DigConfig dcfg = DigConfig{0}) : SensorTmpl(dcfg) {}
};

class AlgSensor : public SensorTmpl<AlgSensor, float,
  DigConfig, AlgPort,
  DefAlgProc>
{
  public:
    AlgSensor(DigConfig dcfg) : SensorTmpl(dcfg) {}
};

class DscSensor : public SensorTmpl<DscSensor, int,
  DigConfig, DigPort,
  DefDigProc>
{
  public:
    DscSensor(DigConfig dcfg) : SensorTmpl(dcfg) {}
};


typedef std::variant<DigSensor::DataTp, AlgSensor::DataTp, DscSensor::DataTp>    vSensorData;

struct SensorDataUpdate
{
  uint8_t index;
  uint8_t s_type;
  vSensorData s_val;
  String name;
};

template<typename S, typename D>
class Publisher
{
    std::vector<S> m_subs;

  public:
    void subscribe(S &s)
    {
      m_subs.push_back(s);
      on_subscribe(s);
    }

    void notify(D &data)
    {
      if (m_subs.size() > 0)
      {
        Serial.println("Notifying...");
      }

      for (auto &s : m_subs)
      {
        s.update(data);
      }
    }

    virtual void on_subscribe(S &s) {}
};

std::array<char, 16> ip_from_int(uint32_t ip_i)
{
  char ip_b[4] = {0, 0, 0, 0};
  std::array<char, 16> ip_s = {0};

  ip_b[3] =  ip_i     & 0xff;
  ip_b[2] = (ip_i >> 8) & 0xff;
  ip_b[1] = (ip_i >> 16) & 0xff;
  ip_b[0] = (ip_i >> 24) & 0xff;

  snprintf(&ip_s[0], 16, "%d.%d.%d.%d", ip_b[0], ip_b[1], ip_b[2], ip_b[3]);

  return ip_s;
}

class WebSubscriberProxy
{
    uint32_t cl_host;
    uint16_t cl_port;

  public:

    WebSubscriberProxy(uint32_t host, uint16_t port, uint8_t idx)
      : cl_host(host)
      , cl_port(port)
    {}

    void update(SensorDataUpdate &d)
    {
      WiFiClient client;
      Serial.print("sending update to: ");
      Serial.println(&ip_from_int(cl_host)[0]);
      client.connect(&ip_from_int(cl_host)[0], cl_port);

      const size_t capacity = JSON_OBJECT_SIZE(4) ; //+ JSON_ARRAY_SIZE(2);

      DynamicJsonDocument req(capacity);

      JsonObject object = req.to<JsonObject>();
      object["index"] = d.index;
      object["type"] =  d.s_type;

      std::visit([this, &object](auto && r) {
        Serial.println(r);
        object["value"] = r;
      }, d.s_val);


      serializeJsonPretty(object, client);

      client.println("");

      delay(300);
      client.stop();
      delay(300);
    }

};



class WebPublisher : public Publisher<WebSubscriberProxy, SensorDataUpdate>
{

    WiFiServer server{2023};

    std::vector<WebSubscriberProxy> proxies;

  public:

    WebPublisher()
    {
      //server.begin();
    }


    void begin()
    {
      server.begin();
    }

    //    void on_subscribe(WebSubscriberProxy &p) override
    //    {
    //    }

    bool checkForSubscriber()
    {
      Serial.println("checking for subscriber");
      //      auto client = server.available();

      WiFiClient client = server.available();

      if (client)
      {
        Serial.println("got client");
        bool currentLineIsBlank = true;

        while (client.connected())
        {
          Serial.println("client connected");
          if (client.available())
          {
            Serial.println("client available");

            const size_t capacity = JSON_OBJECT_SIZE(4) ;//+ JSON_ARRAY_SIZE(2);

            StaticJsonDocument<capacity> req;
            DeserializationError error = deserializeJson(req, client);

            if (error) {
              Serial.print(F("deserializeJson() failed: "));
              Serial.println(error.f_str());
              break;
            }
            else
            {
              Serial.println("client registered");
              Serial.println(&ip_from_int(req["ip"].as<uint32_t>())[0]);
              Serial.println(req["port"].as<uint16_t>());
              Serial.println(req["index"].as<uint8_t>());

              client.println(&ip_from_int(req["ip"].as<uint32_t>())[0]);

              proxies.push_back(WebSubscriberProxy(req["ip"].as<uint32_t>(),
                                                   req["port"].as<uint16_t>(),
                                                   req["index"].as<uint8_t>()
                                                  ));

              subscribe(proxies.back());

            }

            char c = client.read();
            //Serial.write(c);
            if (c == '\n' && currentLineIsBlank) {
              // send a standard http response header
              client.println("success");
              break;
            }
            if (c == '\n') {
              // you're starting a new line
              currentLineIsBlank = true;
            }
            else if (c != '\r') {
              // you've gotten a character on the current line
              currentLineIsBlank = false;
            }

            /* json register format
              {
              "ip":3232257813,
              "port":2024,
              "index":1
              }

            */
          }
          else
          {
            Serial.println("Client not available");
            delay(1000);
          }
        }

        // give the web browser time to receive the data
        delay(1);

        // close the connection:
        client.stop();
        Serial.println("client disconnected");
      }
      else
      {
        Serial.println("no client");
      }
      return false;
    }
};


template<typename ... S>
class sensor_ctrl : public WebPublisher
{
    std::array<vSensor, sizeof...(S)> m_sensors;
    //std::array<Sensor*, sizeof...(S)> m_sensors;
    //std::multimap<int, std::unique_ptr<Sensor>> m_sensors;

  public:

    sensor_ctrl(S...s)
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


auto ms1 = DigSensor();
auto ms2 = AlgSensor(DigConfig{2});

sensor_ctrl ctrl(ms1, ms2, DigSensor(DigConfig{3}));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Wifi configuration
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conecting to: ");
  Serial.println(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected...");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
  //  ctrl = std::make_unique<sensor_ctrl>(ms1, ms2, DigSensor());
  ctrl.begin();

}


void loop() {
  // put your main code here, to run repeatedly:

  ctrl.checkForSubscriber();

  ctrl.update_all();
  delay(1000);
}

