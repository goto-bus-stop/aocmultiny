#include <stdlib.h>
#include <dplobby.h>
#include "DPAddress.hpp"
#include "DPSessionDesc.hpp"
#include "DPName.hpp"
#include "DPLConnection.hpp"

namespace aocmultiny {
namespace dplib {

DPLConnection::DPLConnection (DPAddress address, DPSessionDesc* sessionDesc, DPName* playerName)
    :
    address(address),
    sessionDesc(sessionDesc),
    name(playerName) {
  this->alloc();
}

void DPLConnection::alloc () {
  auto connection = static_cast<DPLCONNECTION*>(malloc(sizeof(DPLCONNECTION)));
  connection->dwSize = sizeof(DPLCONNECTION);
  connection->dwFlags = this->sessionDesc->isHost() ?
    DPLCONNECTION_CREATESESSION : DPLCONNECTION_JOINSESSION;
  connection->lpSessionDesc = this->sessionDesc->unwrap();
  connection->lpPlayerName = this->name->unwrap();
  connection->guidSP = DPSPGUID_TCPIP;

  connection->lpAddress = this->address.address;
  connection->dwAddressSize = this->address.size;
  this->dpConnection = connection;
}

DPLCONNECTION* DPLConnection::unwrap () {
  return this->dpConnection;
}

DPLConnection::~DPLConnection () {
  free(this->dpConnection);
}

}
}
