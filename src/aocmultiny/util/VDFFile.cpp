#include "VDFFile.hpp"
#include <fstream>
#include <map>
#include <string>

namespace aocmultiny {
namespace util {

using std::ifstream;
using std::ios;
using std::streampos;
using std::string;
using std::map;

class VDFNode;
class VDFParser;
class VDFFile;

VDFNode::VDFNode ()
    :
    type(0) {
}

VDFNode::VDFNode (string value)
    :
    type(1),
    value(value) {
}

bool VDFNode::isValue () const {
  return this->type == 1;
}
bool VDFNode::isMap () const {
  return this->type == 0;
}

VDFNode::operator const char* () const {
  return this->value.c_str();
}

// VDFNode VDFNode::operator [] (string key) {
//   return this->children[key];
// }

VDFParser::VDFParser (string contents)
    :
    source(contents) {
}

VDFNode VDFParser::parse () {
  this->i = 0;
  this->size = this->source.size();
  return this->parseTree();
}

VDFNode VDFParser::parseTree () {
  auto tree = VDFNode();
  string key = "";
  bool isValue = false;
  while (this->i < this->size) {
    auto c = this->source[this->i++];

    if (c == '"') {
      auto start = this->i;
      auto end = this->source.find('"', start);
      string str = this->source.substr(start, end - start);
      if (!isValue) {
        isValue = true;
        key = str;
      } else {
        isValue = false;
        tree[key] = str;
      }
      this->i = end + 1;
    }

    if (c == '{') {
      isValue = false;
      tree[key] = this->parseTree();
    }

    if (c == '}') {
      return tree;
    }
  }
  return tree;
}

void VDFFile::read () {
  auto file = ifstream(this->path);
  file.exceptions(ifstream::failbit | ifstream::badbit);
  file.seekg(0, ios::end);
  auto length = file.tellg();
  file.seekg(0, ios::beg);
  auto buf = new char[length];
  file.read(buf, length);
  content = buf;
}

VDFFile::VDFFile (string path)
    :
    path(path) {
  this->read();
}

VDFFile::~VDFFile () {}

VDFNode VDFFile::parse () {
  auto parser = VDFParser(this->content);
  return parser.parse();
}

}
}
