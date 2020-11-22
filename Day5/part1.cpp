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
      else input[s] = 1;
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
  MEASURE(solve(input);, 1, 1);
}
