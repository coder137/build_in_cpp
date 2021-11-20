#include <iostream>

#include "generic_add.h"
#include "generic_vector2.h"

int main() {
  std::cout << "Hello World from test" << std::endl;

  Vector2<int> vi1(2, 4);
  Vector2<int> vi2(3, 6);
  Vector2<int> vi3 = generic_add(vi1, vi2);
  vi3.Print();

  Vector2<float> vf1(1.1, 2.2);
  Vector2<float> vf2(2.2, 3.3);
  Vector2<float> vf3 = generic_add(vf1, vf2);
  vf3.Print();

  return 0;
}
