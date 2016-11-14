#pragma once
#include <string>
#include <map>

namespace aocmultiny {
namespace util {

using std::string;
using std::map;

class VDFNode;
class VDFParser;
class VDFFile;

class VDFNode: public map<string, VDFNode> {
private:
  int type;
  string value;

public:
  VDFNode ();
  VDFNode (string value);

  bool isValue () const;
  bool isMap () const;

  operator const char* () const;
};

class VDFParser {
private:
  string source;
  size_t i;
  size_t size;

public:
  VDFParser (string contents);
  VDFNode parse ();

  VDFNode parseTree ();
};

class VDFFile {
private:
  string path;
  string content;

  void read ();

public:
  VDFFile (string path);
  ~VDFFile ();

  VDFNode parse ();
};

}
}
