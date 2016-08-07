#include "DPAddress.hpp"
#include "DPSessionDesc.hpp"
#include "DPName.hpp"
#include "DPLConnection.hpp"
#include <stdlib.h>
#include <dplobby.h>
#include <tuple>

using std::tie;

namespace dplib {

const GUID DPSPGUID_NICE = { 0xe2dd8ebe, 0x1f03, 0x43b7, { 0x8d, 0x92, 0x9c, 0x6c, 0x2f, 0x5c, 0x44, 0x26 } };

DPLConnection::DPLConnection (DPAddress* address, DPSessionDesc* sessionDesc, DPName* playerName)
    :
    address(address),
    sessionDesc(sessionDesc),
    name(playerName),
    dpConnection(NULL) {
}

void DPLConnection::alloc () {
  auto connection = new DPLCONNECTION;
  connection->dwSize = sizeof(DPLCONNECTION);
  connection->dwFlags = this->sessionDesc->isHost() ?
    DPLCONNECTION_CREATESESSION : DPLCONNECTION_JOINSESSION;
  connection->lpSessionDesc = this->sessionDesc->unwrap();
  connection->lpPlayerName = this->name->unwrap();
  connection->guidSP = DPSPGUID_NICE;

  tie(
    connection->lpAddress,
    connection->dwAddressSize
  ) = this->address->unwrap();

  this->dpConnection = connection;
}

DPLCONNECTION* DPLConnection::unwrap () {
  if (!this->dpConnection) {
    this->alloc();
  }
  return this->dpConnection;
}

DPLConnection::~DPLConnection () {
  if (this->dpConnection) {
    delete this->dpConnection;
    this->dpConnection = NULL;
  }
}

}
