#include "DPAddress.hpp"
#include "DPSessionDesc.hpp"
#include "DPName.hpp"
#include "DPLConnection.hpp"
#include <stdlib.h>
#include <dplobby.h>
#include <tuple>

using std::tie;

namespace dplib {

DPLConnection::DPLConnection (DPAddress* address, DPSessionDesc* sessionDesc, DPName* playerName)
    :
    address(address),
    sessionDesc(sessionDesc),
    name(playerName),
    dpConnection(NULL) {
}

DPAddress* DPLConnection::getAddress () {
  return this->address;
}

DPSessionDesc* DPLConnection::getSessionDesc () {
  return this->sessionDesc;
}

DPName* DPLConnection::getPlayerName () {
  return this->name;
}

void DPLConnection::alloc () {
  auto connection = new DPLCONNECTION;
  connection->dwSize = sizeof(DPLCONNECTION);
  connection->dwFlags = this->sessionDesc->isHost() ?
    DPLCONNECTION_CREATESESSION : DPLCONNECTION_JOINSESSION;
  connection->lpSessionDesc = this->sessionDesc->unwrap();
  connection->lpPlayerName = this->name->unwrap();
  auto spPart = this->address->get(DPAID_ServiceProvider);
  connection->guidSP = *static_cast<GUID*>(spPart.first);

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

DPLConnection* DPLConnection::parse (DPLCONNECTION* raw) {
  auto isHost = raw->dwFlags & DPLCONNECTION_CREATESESSION;
  auto address = DPAddress::parse(raw->lpAddress, raw->dwAddressSize);
  auto sessionDesc = DPSessionDesc::parse(raw->lpSessionDesc, isHost);
  auto playerName = DPName::parse(raw->lpPlayerName);
  return new DPLConnection(address, sessionDesc, playerName);
}

}
