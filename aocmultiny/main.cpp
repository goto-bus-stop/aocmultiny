#include <string>
#include <iostream>
#include "main.hpp"
#include "Lobby.hpp"

using namespace std;

namespace aocmultiny {

void main () {
  CoInitialize(NULL);
  wcout << "[main] Starting" << endl;

  Lobby lobby;

  lobby.host();

  wcout << "[main] Exiting" << endl;

  CoUninitialize();
}

}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {
  aocmultiny::main();
  return 0;
}
