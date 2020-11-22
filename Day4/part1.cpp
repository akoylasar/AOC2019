#include <iostream>

#define INPUT_A 402328
#define INPUT_B 864247

int main()
{
  int result = 0;
  for (int i = INPUT_A; i < INPUT_B; ++i)
  {
    if (i > 999999 || i < 100000) continue;
    int c = i; bool dupl = false; int prev = 10;
    while (c != 0)
    {
      int d = c % 10;
      if (d > prev) break;
      dupl = dupl ? dupl : d == prev; prev = d;
      c /= 10;
    }
    if (dupl && !c) result++;
  }
  std::cout << result << std::endl;
}
