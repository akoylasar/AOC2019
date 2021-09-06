#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>

#include "../common.hpp"

std::vector<int> getInput()
{
  std::vector<int> result;
  
  std::ifstream file("part2.txt", std::ios::in);
  
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
  const int output = 19690720; // from problem description.
  std::vector<int> input = getInput();
  const auto op = [](int x, int y, int c) { return c == 1 ? x + y : x * y; };
  
  bool terminate = false;
  int noun = 0;
  int verb = 50;
  MEASURE(
  for (noun = 0; noun < 100; ++noun)
  {
    int l = 0; int h = 100;
    verb = 50;
    // Here we do a binary search to speed things up with the assumption
    // that the only operations supported are + and * on non-negative integers.
    while (l < h)
    {
      // Resetting the program per run.
      std::vector<int> s = input;
      s[1] = noun; s[2] = verb; // Input that differs per run.
      for (unsigned i = 0; s[i] != 99; i = i + 4)
        s[s[i + 3]] = op(s[s[i + 1]], s[s[i + 2]], s[i]);
      int x = s[0];
      if (x == output)
      {
        terminate = true;
        break;
      }
      x < output ? (l = verb) : (h = verb);
      verb = (h - l + 1) / 2 + l;
    }
    if (terminate) break;
  };, 1);
  std::cout << "noun: " << noun << " verb: " << verb << " result: " << 100 * noun + verb << "\n";
}
