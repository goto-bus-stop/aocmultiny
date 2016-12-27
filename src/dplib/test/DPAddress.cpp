#include <catch.hpp>
#include <dplay.h>
#include "../DPAddress.hpp"
#include "../DPLobby.hpp"

using std::string;
using std::tie;

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

BOOL FAR PASCAL stubEnumAddressCallback (REFGUID type, DWORD size, const void* data, void*) {
  return true;
}

TEST_CASE ("Allocate DirectPlay compound address", "[DPAddress]") {
  auto address = dplib::DPAddress::ip("8.8.8.8");
  auto lobby = dplib::DPLobby::get()->getInternalLobby();
  void* addressData;
  DWORD addressSize;
  tie(addressData, addressSize) = address->unwrap();
  CHECK(lobby->EnumAddress(stubEnumAddressCallback, addressData, addressSize, NULL) == DP_OK);
}

TEST_CASE ("Parsing DirectPlay compound addresses", "[DPAddress]") {
  string expectedIp = "1.2.3.4";
  auto address = dplib::DPAddress::ip(expectedIp);
  void* addressData;
  DWORD addressSize;
  tie(addressData, addressSize) = address->unwrap();

  address = dplib::DPAddress::parse(addressData, addressSize);
  auto ipPart = address->get(DPAID_INet);
  auto parsedIp = static_cast<char*>(ipPart.first);
  CHECK(string(parsedIp) == expectedIp);
}
