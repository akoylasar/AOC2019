#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "../common.hpp"

#define PHASE_BASE 5

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

inline int fact(int n)
{
  if (n == 0) return 1;
  int result = 1;
  for (int i = 0; i < n; ++i)
    result *= i;
  return result;
}

inline int arOp(int x, int y, int c) { return c == 1 ? x + y : x * y; }
inline bool jmpOp(int x, int c)
{
  return (c == 5) ? (x != 0) : (x == 0);
}
inline int compOp(int x, int y, int c)
{
  return static_cast<int>(((c == 7) ? (x < y) : (x == y)));
}

using Stack = std::vector<int>;
using PhaseSeq = std::vector<int>;

void compute(std::vector<int>& input, Stack& stack)
{
  int output;
  for (unsigned i = 0;;)
  {
    const int inst = input[i];
    const int op = inst % 10;
    if (op == 9)
    {
      stack.push_back(output);
      break;
    }
    if (op == 3 || op == 4) // mem op
    {
      const int s = input[i + 1];
      if (op == 4) output = (inst / 100 ? s : input[s]);
      else
      {
        input[s] = stack.back();
        stack.pop_back();
      }
      i += 2;
    }
    else
    {
      const int v0 = input[i + 1];
      const int v1 = input[i + 2];
      const int x = ((inst / 100) % 10) ? v0 : input[v0];
      const int y = (inst / 1000) ? v1 : input[v1];
      if (op == 1 || op == 2) // arith op
      {
        input[input[i + 3]] = arOp(x, y, op);
        i += 4;
      }
      else if (op == 5 || op == 6) // jmp
      {
        i = jmpOp(x, op) ? y : i + 3;
      }
      else // comp op
      {
        input[input[i + 3]] = compOp(x, y, op);
        i += 4;
      }
    }
  }
}

inline std::vector<PhaseSeq> buildPhaseSecList()
{
  // https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
  std::vector<PhaseSeq> result;
  result.reserve(fact(PHASE_BASE));
  PhaseSeq base; base.reserve(PHASE_BASE);
  for (int i = 0; i < PHASE_BASE; ++i) base.push_back(i);
  result.push_back(base);
  while (true)
  {
    int k = -1;
    for (int i = 0; i < PHASE_BASE - 1; ++i)
      k = (base[i] < base[i + 1]) ? (std::max(k, i)) : k;
    if (k == -1)
      break;
    int l = 0;
    for (int i = k + 1; i < PHASE_BASE; ++i)
      l = (base[k] < base[i]) ? (std::max(l, i)) : l;
    std::swap(base[k], base[l]);
    std::reverse(base.begin() + k + 1, base.end());
    result.push_back(base);
  }
  return result;
}

void solve(const std::vector<int>& originalInput)
{
  std::vector<PhaseSeq> phaseSeqList = buildPhaseSecList();
  int maxThrusterSignal = 0;
  for (auto& phaseSeq : phaseSeqList)
  {
    Stack stack { 0 };
    for (auto phase : phaseSeq)
    {
      stack.push_back(phase);
      auto input = originalInput; // reset input.
      compute(input, stack);
    }
    maxThrusterSignal = std::max(stack.back(), maxThrusterSignal);
    stack.pop_back();
  }
  std::cout << "Max threshold signal: " << maxThrusterSignal << "\n";
}

int main()
{
  std::vector<int> input = getInput();
  MEASURE(solve(input);, 1, 1);
}
