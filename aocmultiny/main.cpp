#include <stdio.h>
#include <windows.h>
#include "main.hpp"

namespace aocmultiny {

void main () {
  printf("Hello World\n");
}

}

int WINAPI WinMain (HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {
  aocmultiny::main();
}
