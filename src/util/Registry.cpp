#include "Registry.hpp"
#include <iostream>

namespace aocmultiny {
namespace util {

wstring stow (string s) {
  wstring w;
  for (auto c : s) {
    w.push_back(c);
  }
  return w;
}

string wtos (wstring w) {
  string s;
  for (auto c : w) {
    s.push_back(static_cast<char>(c));
  }
  return s;
}

Registry::Registry (HKEY key)
    :
    key(key),
    path(L"") {
}
Registry::Registry (HKEY key, wstring path)
    :
    key(key),
    path(path) {
}

Registry::~Registry () {
  RegCloseKey(this->key);
}

Registry Registry::getSubKey (wstring keyName) {
  HKEY subkey;
  auto hr = RegOpenKeyEx(this->key, keyName.c_str(), 0, KEY_READ, &subkey);
  if (hr != ERROR_SUCCESS) {
    throw std::out_of_range("Registry key " + wtos(keyName) + " does not exist.");
  }
  return Registry(subkey);
}

Registry Registry::operator[] (wstring key) {
  return this->getSubKey(key);
}

wstring Registry::readString (wstring key) {
  DWORD strLen;
	wchar_t* result;
	auto err = RegGetValue(this->key, NULL, key.c_str(), RRF_RT_ANY, NULL, NULL, &strLen);
	if (err != ERROR_SUCCESS) {
    return NULL;
  }
	result = static_cast<wchar_t*>(malloc(strLen));
	err = RegGetValue(this->key, NULL, key.c_str(), RRF_RT_ANY, NULL, result, &strLen);
  return result;
}

// narrow string compat
Registry Registry::operator[] (string key) {
  auto wkey = stow(key);
  return this->getSubKey(wkey);
}
wstring Registry::readString (string key) {
  return this->readString(stow(key));
}

}
}
