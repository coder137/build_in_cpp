#include <math.h>
#include <stdio.h>

int main() {
  printf("Hello World\r\n");

#if RANDOM == 1
  printf("Random == 1\r\n");
#endif

  float f1 = 10.2;
  float f2 = 1.0;
  printf("Input a floating point number\r\n");
  scanf("%f", &f2);

  float value = pow(f1, f2);
  printf("Computed pow: %f\r\n", value);

  return 0;
}
