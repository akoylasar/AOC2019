#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>

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

#define PHASE_BASE 5
#define PHASE_END 9

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
using Program = std::vector<int>;
using PhaseSeq = std::vector<int>;

struct Amplifier
{
  int ip;
  Stack input;
  Program program;
  Stack output;
};

using AmpPtr = std::shared_ptr<Amplifier>;

bool compute(AmpPtr amp)
{
  unsigned int i = amp->ip;
  while (1)
  {
    const int inst = amp->program[i];
    const int op = inst % 10;
    if (inst == 99) break;
    if (op == 3 || op == 4) // mem op
    {
      const int s = amp->program[i + 1];
      if (op == 4)
      {
        amp->output.push_back(inst / 100 ? s : amp->program[s]);
      }
      else
      {
        if (!amp->input.empty())
        {
          amp->program[s] = amp->input.back();
          amp->input.pop_back();
          
        }
        else
        {
          amp->ip = i;
          return false;
        }
      }
      i += 2;
    }
    else
    {
      const int v0 = amp->program[i + 1];
      const int v1 = amp->program[i + 2];
      const int x = ((inst / 100) % 10) ? v0 : amp->program[v0];
      const int y = (inst / 1000) ? v1 : amp->program[v1];
      if (op == 1 || op == 2) // arith op
      {
        amp->program[amp->program[i + 3]] = arOp(x, y, op);
        i += 4;
      }
      else if (op == 5 || op == 6) // jmp
      {
        i = jmpOp(x, op) ? y : i + 3;
      }
      else // comp op
      {
        amp->program[amp->program[i + 3]] = compOp(x, y, op);
        i += 4;
      }
    }
  }
  amp->ip = i;
  return true;
}

inline std::vector<PhaseSeq> buildPhaseSecList(int a, int b)
{
  // https://en.wikipedia.org/wiki/Permutation#Generation_in_lexicographic_order
  std::vector<PhaseSeq> result;
  int n = b - a + 1;
  result.reserve(fact(n));
  PhaseSeq base; base.reserve(n);
  for (int i = a; i <= b; ++i) base.push_back(i);
  result.push_back(base);
  while (true)
  {
    int k = -1;
    for (int i = 0; i < n - 1; ++i)
      k = (base[i] < base[i + 1]) ? (std::max(k, i)) : k;
    if (k == -1)
      break;
    int l = 0;
    for (int i = k + 1; i < n; ++i)
      l = (base[k] < base[i]) ? (std::max(l, i)) : l;
    std::swap(base[k], base[l]);
    std::reverse(base.begin() + k + 1, base.end());
    result.push_back(base);
  }
  return result;
}

void solve(const std::vector<int>& program)
{
  std::vector<PhaseSeq> phaseSeqList = buildPhaseSecList(PHASE_BASE, PHASE_END);
  int maxThrusterSignal = 0;
  for (const auto& phaseSeq : phaseSeqList)
  {
    std::vector<AmpPtr> amps;
    amps.resize(PHASE_BASE);
    for (int i = 0; i < PHASE_BASE; ++i)
    {
      amps[i] = std::make_shared<Amplifier>();
      amps[i]->ip = 0;
      amps[i]->input.clear();
      amps[i]->input.push_back(phaseSeq[i]);
      amps[i]->program = program;
      amps[i]->output.clear();
    }
    amps[0]->input.insert(amps[0]->input.begin(), 0);
    int i = 0;
    while (true)
    {
      const bool c = compute(amps[i]);
      if (c && i == PHASE_BASE - 1) break;
      auto amp = amps[i];
      auto nextAmp = amps[(i + 1) % PHASE_BASE];
      nextAmp->input.insert(nextAmp->input.begin(), amp->output.back());
      amp->output.pop_back();
      ++i;
      i = i % PHASE_BASE;
    }
    maxThrusterSignal = std::max(amps[PHASE_BASE - 1]->output.back(), maxThrusterSignal);
  }
  std::cout << "Max threshold signal: " << maxThrusterSignal << "\n";
}

int main()
{
  std::vector<int> input = getInput();
  MEASURE(solve(input);, 1, 1);
}
