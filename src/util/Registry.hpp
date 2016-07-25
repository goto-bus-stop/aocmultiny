#pragma once
#include <string>
#include <windows.h>

using std::string;
using std::wstring;

namespace aocmultiny {
namespace util {

class Registry {
private:
  HKEY key;
  wstring path;

public:
  Registry (HKEY key);
  Registry (HKEY key, wstring path);
  ~Registry ();

  Registry getSubKey (wstring keyName);

  Registry operator[] (wstring key);
  Registry operator[] (string key);

  wstring readString (wstring key);
  wstring readString (string key);
};

}
}
