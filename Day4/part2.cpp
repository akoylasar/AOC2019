#include <iostream>
#include <cmath>
#include <map>

#define INPUT_A 402328
#define INPUT_B 864247

int main()
{
  int result = 0;
  for (int i = INPUT_A; i < INPUT_B; ++i)
  {
    if (i > 999999 || i < 100000) continue;
    std::map<char, char> rec; // @todo: optimise.
    int c = i; int prev = 10;;
    while (c != 0)
    {
      int d = c % 10;
      if (d > prev) break;
      if (d == prev) rec[d]++;
      prev = d;
      c /= 10;
    }
    if (!c)
    {
      for (const auto& r : rec)
        if (r.second == 1)
        {
          ++result;
          break;
        }
    }
  }
  std::cout << result << std::endl;
}
