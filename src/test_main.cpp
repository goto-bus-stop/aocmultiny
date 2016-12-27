#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main(int argc, const char* argv[]) {
  CoInitialize(NULL);

  int result = Catch::Session().run(argc, argv);

  CoUninitialize();
  return result;
}
