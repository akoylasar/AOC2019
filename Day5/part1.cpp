#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>

#include "../common.hpp"

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

void solve(std::vector<int>& input)
{
  const auto arOp = [](int x, int y, int c) { return c == 1 ? x + y : x * y; };
  for (unsigned i = 0;;)
  {
    const int inst = input[i];
    const int op = inst % 10;
    if (op == 9) break;
    const bool memOp = (op == 3 || op == 4);
    if (memOp)
    {
      const int s = input[i + 1];
      if (op == 4) std::cout << (inst / 100 ? s : input[s]) << "\n";
      else input[s] = 1; //  providing 1 to the only input instruction
      i += 2;
    }
    else
    {
      const int v0 = input[i + 1];
      const int v1 = input[i + 2];
      const int x = ((inst / 100) % 10) ? v0 : input[v0];
      const int y = (inst / 1000) ? v1 : input[v1];
      input[input[i + 3]] = arOp(x, y, op);
      i += 4;
    }
  }
  std::cout << std::endl;
}

int main()
{
  std::vector<int> input = getInput();
  MEASURE(solve(input);, 1);
}
