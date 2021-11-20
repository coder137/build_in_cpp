#include <iostream>

#include "generic_add.h"

int main() {
  std::cout << "Hello World from test" << std::endl;

  int int_add = generic_add(1, 2);
  std::cout << "INT ADD: " << int_add << std::endl;

  float float_add = generic_add(1.1, 4.5);
  std::cout << "FLOAT ADD: " << float_add << std::endl;

  return 0;
}
