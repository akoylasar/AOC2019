#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>

#define MEASURE(expression, numTrials, numRuns)\
{\
using DDuration = std::chrono::duration<double, std::milli>;\
double duration = 0.0;\
for (int _trial = 0; _trial < numTrials; ++_trial)\
{\
const auto _t0 = std::chrono::high_resolution_clock::now();\
for (int _run = 0; _run < numRuns; ++_run)\
{\
expression\
}\
const auto _t1 = std::chrono::high_resolution_clock::now();\
duration += std::chrono::duration_cast<DDuration>(_t1 - _t0).count();\
}\
const double avgDuration = duration / (numTrials * numRuns);\
std::cout << "Ran " << numTrials << " trial(s) each of which consisting of " << numRuns << " run(s). Average time of the algorithm is " << avgDuration << "(ms).\n";\
}

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
  const int output = 19690720;
  std::vector<int> input = getInput();
  const auto op = [](int x, int y, int c) { return c == 1 ? x + y : x * y; };
  
  MEASURE(
  bool terminate = false;
  int noun = 0;
  int verb = 50;
  for (noun = 0; noun < 100; ++noun)
  {
    int l = 0; int h = 100;
    verb = 50;
    while (l < h)
    {
      std::vector<int> s = input;
      s[1] = noun; s[2] = verb;
      for (unsigned i = 0; s[i] != 99; i = i + 4)
        s[input[i + 3]] = op(s[s[i + 1]], s[s[i + 2]], s[i]);
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
  }
  std::cout << "noun: " << noun << " verb: " << verb << " result: " << 100 * noun + verb << "\n";
  ,1, 1);
}
