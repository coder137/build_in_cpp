#include <iostream>
#include <math.h>

#include "random.h"

int main() {
  std::cout << "Hello World from test" << std::endl;

  // -Wall -Werror flags throw error when unused variable
  // int i = 0;

#if RANDOM == 1
  random_print();
#endif

  float f1 = 10.2;
  float f2 = 2.3;

  float value = pow(f1, f2);
  std::cout << value << std::endl;

  return 0;
}
