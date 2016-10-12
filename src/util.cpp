#include "util.hpp"
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace std {

wstring to_wstring (GUID guid) {
  wchar_t* chars;
  StringFromIID(guid, &chars);
  wstring wstr = chars;
  CoTaskMemFree(chars);
  return wstr;
};

string to_string (GUID guid) {
  wchar_t* chars;
  StringFromIID(guid, &chars);
  int l = wcslen(chars);
  string str;
  for (int i = 0; i < l; i++) {
    str.push_back(chars[i]);
  }
  CoTaskMemFree(chars);
  return str;
}

}

namespace aocmultiny {

vector<string> split (const string &s, char delim) {
  stringstream ss (s);
  string item;
  vector<string> elems;
  while (getline(ss, item, delim)) {
    elems.push_back(move(item));
  }
  return elems;
}

}
