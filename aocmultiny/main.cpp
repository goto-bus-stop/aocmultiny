#include "main.hpp"
#include "Lobby.hpp"

namespace aocmultiny {

void main () {
  CoInitialize(NULL);
  printf("Starting\n");

  Lobby lobby;

  lobby.host();

  printf("Did stuff!\n");

  CoUninitialize();
}

}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {
  aocmultiny::main();
  return 0;
}
