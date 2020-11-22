#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>

std::vector<int> getInput()
{
  std::vector<int> result;
  
  std::ifstream file("part2.txt", std::ios::in);
  
  std::string line;
  while (std::getline(file, line))
  {
    int mass = std::stoi(line);
    result.push_back(mass);
  }
  
  return result;
}

int main()
{
  const std::vector<int> input = getInput();
  const auto getTotalFuel = [](int mass) -> int
  {
    int result = 0;
    while (true)
    {
      mass = mass / 3 - 2;
      if (mass <= 0)
        break;
      result += mass;
    }
    return result;
  };
  std::cout << std::reduce(input.begin(), input.end(), 0, [getTotalFuel](int a, int b) { return a + getTotalFuel(b); }) << "\n";
}
