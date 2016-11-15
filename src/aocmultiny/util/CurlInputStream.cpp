#include "CurlInputStream.hpp"
#include <curl/curl.h>
#include <process.h>
#include <algorithm>
#include <map>
#include <string>
#include <iostream>

namespace aocmultiny {
namespace util {

using std::map;
using std::stoi;
using std::string;
using std::transform;

static size_t onheader (char* data, size_t size, size_t nitems, void* ctx) {
  auto download = static_cast<CurlInputStream*>(ctx);
  auto header = string(data, size * nitems);
  auto index = header.find(':');
  if (index != string::npos) {
    auto name = header.substr(0, index);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    download->setHeader(
      name,
      header.substr(index + 2)
    );
  }
  return size * nitems;
}

static size_t onprogress (char* data, size_t size, size_t nmemb, void* ctx) {
  auto download = static_cast<CurlInputStream*>(ctx);
  return download->receive(data, size * nmemb);
}

static void downloadThread (void* ctx) {
  auto stream = static_cast<CurlInputStream*>(ctx);
  std::cout << "curl_easy_perform" << std::endl;
  curl_easy_setopt(stream->getCurl(), CURLOPT_VERBOSE, 1L);
  curl_easy_perform(stream->getCurl());
  std::cout << "curl_easy_performed" << std::endl;
  stream->setFinished();
  _endthread();
}

CurlInputBuffer::CurlInputBuffer (size_t maxSize) {
  this->buffer = new char[maxSize];
  this->maxSize = maxSize;
  this->size = 0;
  this->ptr = 0;
}

CurlInputBuffer::~CurlInputBuffer () {
  delete this->buffer;
}

bool CurlInputBuffer::append (void* data, size_t size) {
  wxMutexLocker lock (this->lock);

  // std::cout << "[CurlInputBuffer::append] " << size << " size=" << this->size << " max=" << this->maxSize << std::endl;

  if (this->maxSize - this->ptr < size * 2) {
    this->realign();
  }

  if (this->spaceLeft() < size) {
    return false;
  }
  memcpy(&this->buffer[this->size], data, size);
  this->size += size;
  return true;
}

size_t CurlInputBuffer::read (void* output, size_t size) {
  wxMutexLocker lock (this->lock);

  // std::cout << "[CurlInputBuffer::read] " << size << " unread=" << this->unread() << std::endl;
  if (this->unread() < size) {
    size = this->unread();
    if (size <= 0) {
      return 0;
    }
  }
  memcpy(output, &this->buffer[this->ptr], size);
  this->ptr += size;

  return size;
}

size_t CurlInputBuffer::unread () {
  wxMutexLocker lock (this->lock);

  return this->size - this->ptr;
}

size_t CurlInputBuffer::spaceLeft () {
  wxMutexLocker lock (this->lock);

  return this->maxSize - this->size;
}

void CurlInputBuffer::realign () {
  wxMutexLocker lock (this->lock);

  // std::cout << "[CurlInputBuffer::realign] ptr=" << this->ptr << " size=" << this->size << " max=" << this->maxSize << std::endl;
  // TODO reuse the same buffer here instead of creating a new one. The bytes
  // that have to be kept can just be moved to the front of the old buffer
  // instead of in a new buffer.
  auto oldBuffer = this->buffer;
  auto reuseData = &this->buffer[this->ptr];
  auto reuseSize = this->unread();
  auto newBuffer = new char[this->maxSize];
  memcpy(newBuffer, reuseData, reuseSize);
  this->ptr = 0;
  this->size = reuseSize;
  this->buffer = newBuffer;
  delete oldBuffer;
}

CurlInputStream::CurlInputStream ()
    :
    curl(curl_easy_init()),
    receivedSize(0),
    paused(false),
    isFinished(false) {
  curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, onprogress);
  curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, onheader);
  curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, this);
  curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "");
  this->buffer = new CurlInputBuffer();
}

CurlInputStream::~CurlInputStream () {
  CloseHandle(this->onReading);
  CloseHandle(this->thread);
  this->wait();
  delete this->buffer;
  curl_easy_cleanup(this->curl);
}

void CurlInputStream::pause (bool inWriteFunc) {
  this->paused = true;
  if (!inWriteFunc) {
    curl_easy_pause(this->curl, CURLPAUSE_RECV);
  }
}

void CurlInputStream::resume () {
  this->paused = false;
  curl_easy_pause(this->curl, ~CURLPAUSE_RECV);
}

string CurlInputStream::getHeader (string name) {
  try {
    return this->headers[name];
  } catch (std::out_of_range) {
    return "";
  }
}

void CurlInputStream::setHeader (string name, string value) {
  std::cout << "[CurlInputStream::setHeader] " << name << ": " << value;
  this->headers[name] = value;
}

size_t CurlInputStream::receive (char* data, size_t size) {
  SetEvent(this->onReading);
  ResetEvent(this->onReading);
  this->receivedSize += size;
  if (this->buffer->append(data, size)) {
    return size;
  }
  this->pause(true);
  return CURL_WRITEFUNC_PAUSE;
}

void CurlInputStream::get (string url) {
  curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
  this->onReading = CreateEvent(NULL, TRUE, FALSE, NULL);
  this->isFinished = false;
  this->thread = (HANDLE) _beginthread(downloadThread, 0, this);
}

void CurlInputStream::wait () {
  std::cout << "[CurlInputStream::wait]" << std::endl;
  WaitForSingleObject(this->thread, INFINITE);
}

void CurlInputStream::setFinished () {
  std::cout << "[CurlInputStream::setFinished]" << std::endl;
  this->isFinished = true;
}

CURL* CurlInputStream::getCurl () const {
  return this->curl;
}

bool CurlInputStream::Eof () {
  return this->isFinished && this->buffer->unread() == 0;
}

bool CurlInputStream::CanRead () {
  std::cout << "[CurlInputStream::CanRead] " << this->buffer->unread() << std::endl;
  return this->buffer->unread() > 0;
}

size_t CurlInputStream::OnSysRead (void* output, size_t size) {
  if (this->isFinished) {
    return 0;
  }

  if (this->paused && this->buffer->unread() < size) {
    this->resume();
  }

  if (this->buffer->unread() == 0) {
    WaitForSingleObject(this->onReading, 10 * 1000);
  }

  return this->buffer->read(output, size);
}

size_t CurlInputStream::GetSize () {
  auto contentSize = this->getHeader("content-length");
  if (contentSize.empty()) {
    return this->receivedSize;
  }
  return stoi(contentSize);
}

}
}
