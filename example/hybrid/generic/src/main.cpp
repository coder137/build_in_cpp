#include <iostream>

#include "foo.h"

int main() {
  std::cout << "Hello World" << std::endl;
  vFoo();
  std::cout << iFoo() << std::endl;
  std::cout << fFoo(11) << std::endl;
  return 0;
}
