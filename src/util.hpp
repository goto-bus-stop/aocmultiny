#pragma once
#include <objbase.h>
#include <string>
#include <vector>
#include <functional>

using namespace std;

namespace std {

wstring to_wstring (GUID guid);
string to_string (GUID guid);

}

namespace aocmultiny {

vector<string> split (const string &s, char delim);

template <typename ...Args>
class EventListeners {
  typedef function<void(Args...)> Listener;

private:
  vector<Listener> listeners;

public:
  EventListeners () : listeners(vector<Listener> ()) {}

  void emit (Args... args) {
    for (auto l : this->listeners) {
      l(args...);
    }
  }

  EventListeners& operator+= (auto l) {
    this->listeners.push_back(l);
    return *this;
  }
};

}
