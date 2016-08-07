#include <catch.hpp>
#include <dplay.h>
#include "../DPAddress.hpp"

TEST_CASE ("Reading address parts", "[DPAddress]") {
  auto address = new dplib::DPAddress();
  auto length = 4;
  char* data = "Test";
  GUID guid; CoCreateGuid(&guid);
  address->add(guid, data, length);
  auto part = address->get(guid);
  CHECK(part.second == length);
  CHECK(memcmp(part.first, data, length) == 0);
}

TEST_CASE ("Creating IPv4 addresses", "[DPAddress]") {
  char* testIp = "127.0.0.1";
  auto address = dplib::DPAddress::ip(testIp);
  auto ipPart = address->get(DPAID_INet);
  CHECK(strcmp(static_cast<char*>(ipPart.first), testIp));
}
