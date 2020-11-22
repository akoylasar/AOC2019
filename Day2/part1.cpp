#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>

std::vector<int> getInput()
{
  std::vector<int> result;

  std::ifstream file("part1.txt", std::ios::in);
  
  std::string line;
  while (std::getline(file, line, ','))
  {
    int instruction = std::stoi(line);
    result.push_back(instruction);
  }
  
  return result;
}

int main()
{
  std::vector<int> input = getInput();
  input[1] = 12; input[2] = 2; // from problem description.
  const auto op = [](int x, int y, int c) { return c == 1 ? x + y : x * y; };
  for (unsigned i = 0; input[i] != 99; i = i + 4)
    input[input[i + 3]] = op(input[input[i + 1]], input[input[i + 2]], input[i]);
  std::cout << input[0] << "\n";
}
