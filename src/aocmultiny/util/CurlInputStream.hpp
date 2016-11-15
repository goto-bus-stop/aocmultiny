#pragma once
#include <curl/curl.h>
// #include <wx/msw/winundef.h>
#include <wx/stream.h>
#include <wx/thread.h>
#include <algorithm>
#include <map>
#include <string>

namespace aocmultiny {
namespace util {

using std::map;
using std::string;

class CurlHeaders: public map<string, string> {};

class CurlInputBuffer {
private:
  wxMutex lock;
public:
  char* buffer;
  /**
   * Read pointer.
   */
  size_t ptr;
  /**
   * Size of the data in the buffer. Basically a write pointer.
   */
  size_t size;
  /**
   * Size of the buffer.
   */
  size_t maxSize;

  CurlInputBuffer (size_t maxSize = 131072); // 128KB
  ~CurlInputBuffer ();

  bool append (void* data, size_t size);
  size_t read (void* output, size_t size);
  size_t unread ();
  size_t spaceLeft ();
  void realign ();
};

class CurlInputStream: public wxInputStream {
private:
  CURL* curl;
  CurlInputBuffer* buffer;
  size_t receivedSize;
  CurlHeaders headers;
  HANDLE thread;
  HANDLE onReading;
  bool paused;
  bool isFinished;

protected:
  size_t OnSysRead (void* output, size_t size);

  void pause (bool inWriteFunc = false);
  void resume ();

public:
  CurlInputStream ();
  ~CurlInputStream ();

  string getHeader (string name);
  void setHeader (string name, string value);
  size_t receive (char* data, size_t size);
  void get (string url);
  void wait ();

  void setFinished ();
  CURL* getCurl () const;

  virtual bool Eof ();
  virtual bool CanRead ();
  virtual size_t GetSize ();
};

}
}
