#pragma once

#include <array>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "Publisher.hpp"
#include "SensorDataConfig.hpp"

//
// data struct for sharing sensor data
//
struct SensorDataUpdate
{
  uint8_t index;
  uint8_t s_type;
  vSensorData s_val;
  String name;
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
    uint8_t  sns_idx;

  public:

    WebSubscriberProxy(uint32_t host, uint16_t port, uint8_t idx)
      : cl_host(host)
      , cl_port(port)
      , sns_idx(idx)
    {}

    void update(SensorDataUpdate &d)
    {
      if (d.index != sns_idx)
      {
        return;
      }
      
      WiFiClient client;
      Serial.print("sending update to: ");
      Serial.print(&ip_from_int(cl_host)[0]);
      Serial.print(":");
      Serial.println(cl_port);
      
      client.connect(&ip_from_int(cl_host)[0], cl_port);

      const size_t capacity = JSON_OBJECT_SIZE(4) ; //+ JSON_ARRAY_SIZE(2);

      DynamicJsonDocument req(capacity);

      JsonObject object = req.to<JsonObject>();
      object["index"] = d.index;
      object["type"] =  d.s_type;

      Serial.println(d.name);

      std::visit([this, &object](auto && r) {
        Serial.println(r);
        object["value"] = r;
      }, d.s_val);

//      object["name"] = String(d.name);

      serializeJsonPretty(object, client);

      client.println("");

      delay(50);
      client.stop();
      delay(50);
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

    void wifiSetup(const char *ssid, const char *pw)
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pw);
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
              "ip":3232237448,
              "port":2024,
              "index":0
              }

            */
          }
          else
          {
            Serial.println("Client not available");
            delay(100);
          }
        }

        // give the web browser time to receive the data
        delay(10);

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
