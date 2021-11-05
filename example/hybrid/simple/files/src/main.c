#include <math.h>
#include <stdio.h>

int main() {
  printf("Hello World\r\n");

  // -Wall -Werror throw error when unused variable
  //   int i = 0;

#if RANDOM == 1
  printf("Random == 1\r\n");
#endif

  float f1 = 10.2;
  float f2 = 2.3;

  float value = pow(f1, f2);
  printf("Computed pow: %f\r\n", value);

  return 0;
}
