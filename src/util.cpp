#include <string>
#include <vector>
#include <sstream>
#include "util.hpp"

using namespace std;

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
