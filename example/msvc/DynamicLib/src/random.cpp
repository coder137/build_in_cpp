#include "random.h"

#include <iostream>
#include <windows.h>

__declspec(dllexport) void random_func() {
  std::cout << __FUNCTION__ << std::endl;
}
