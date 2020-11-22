#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>

std::vector<int> getInput()
{
  std::vector<int> result;
  
  std::ifstream file("part1.txt", std::ios::in);
  
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
  std::cout << std::reduce(input.begin(), input.end(), 0, [](int a, int b) { return a + (b / 3 - 2); }) << "\n";
}
