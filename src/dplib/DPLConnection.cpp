#include <stdlib.h>
#include <dplobby.h>
#include "DPAddress.hpp"
#include "DPSessionDesc.hpp"
#include "DPName.hpp"
#include "DPLConnection.hpp"

namespace aocmultiny {
namespace dplib {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

DPLConnection::DPLConnection (DPAddress address, DPSessionDesc* sessionDesc, DPName* playerName)
    :
    address(address),
    sessionDesc(sessionDesc),
    name(playerName) {
  this->alloc();
}

void DPLConnection::alloc () {
  auto connection = new DPLCONNECTION;
  connection->dwSize = sizeof(DPLCONNECTION);
  connection->dwFlags = this->sessionDesc->isHost() ?
    DPLCONNECTION_CREATESESSION : DPLCONNECTION_JOINSESSION;
  connection->lpSessionDesc = this->sessionDesc->unwrap();
  connection->lpPlayerName = this->name->unwrap();
  connection->guidSP = DPSPGUID_NICE;

  connection->lpAddress = this->address.address;
  connection->dwAddressSize = this->address.size;
  this->dpConnection = connection;
}

DPLCONNECTION* DPLConnection::unwrap () {
  return this->dpConnection;
}

DPLConnection::~DPLConnection () {
  delete this->dpConnection;
}

}
}
