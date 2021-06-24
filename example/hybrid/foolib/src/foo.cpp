#include "foo.h"

#include <iostream>

EXPORT void vFoo() { std::cout << __FUNCTION__ << std::endl; }

EXPORT int iFoo() {
  std::cout << __FUNCTION__ << std::endl;
  return 11;
}

EXPORT float fFoo(int integer) {
  std::cout << __FUNCTION__ << std::endl;
  return (integer * 1.1);
}
