#pragma once

#include <vector>

template<typename S, typename D>
class Publisher
{
    std::vector<S*> m_subs;

  public:
    void subscribe(S &s)
    {
      m_subs.push_back(&s);
    }

    void notify(D &data)
    {
      if (m_subs.size() > 0)
      {
        Serial.println("Notifying...");
      }

      for (auto &s : m_subs)
      {
        s->update(data);
      }
    }

};
