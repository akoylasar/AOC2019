#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <utility>

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

#define PRINT_OUTPUT 1

using LONG = long long;
static_assert(sizeof(LONG) >= 8, "LONG not supported."); // yeah later...
using RawInput = std::vector<LONG>;
using Output = std::vector<LONG>;

class Input
{
public:
  Input(const RawInput& rawInput) : mMain(rawInput) { }
  LONG& operator[](LONG adr)
  {
    if (adr < mMain.size()) return mMain[adr];
    return mExtraMem[adr];
  }
  const RawInput& getMain() { return mMain; }
private:
  RawInput mMain;
  std::map<LONG, LONG> mExtraMem;
};

struct Vec2 { int x = 0; int y = 0;};
bool operator==(const Vec2& l, const Vec2& r) { return l.x == r.x && l.y == r.y; }

std::size_t FouadHash(const Vec2 v)
{
  int n = v.x + v.y;
  int m = n % 2 ? ((n / 2) * (n + 1)) : (((n - 1) / 2) * n); ++m;
  return m - v.x;
}
namespace std
{
  template<> struct hash<Vec2>
  {
    std::size_t operator()(Vec2 const& v) const noexcept { return FouadHash(v); }
  };
}

class Image
{
public:
  Image(int w, int h)
  : mData(w * h, 0), mW(w), mH(h)
  {
  }
  void set(int x, int y, char value)
  {
    int idx = y * mW + x;
    mData[idx] = value;
  }
  void saveToFile(const char* fileName)
  {
    std::ofstream file(fileName, std::ofstream::binary);
    for (int i = 0; i < mH; ++i)
    {
      for (int j = mW - 1; j >= 0; --j)
      {
        int idx = i * mW + j;
        file << (char)(mData[idx]);
      }
      file << "\n";
    }
  }
private:
  std::vector<char> mData; int mW; int mH;
};

enum TileType
{
  Empty,
  Wall,
  Block,
  Paddle,
  Ball
};

class Cabinet
{
public:
  Cabinet()
  {
  }
  
  void in(LONG in)
  {
    switch (++mIndex) {
      case 3:
      {
        if (mPos.x == -1 && mPos.y == 0)
          mScore = in; // Second cond is really not needed but here anyways.
        else
        {
          auto it = mTiles.find(mPos);
          if (it != mTiles.end())
          {
            if (it->second == Block && (TileType)in == Ball)
              it->second = Empty;
            if ((TileType)in == Ball)
            {
              if (it->second == Block) it->second = Empty; // We don't really need this, but keep for visualisation.
              mBallPos = mPos;
            }
            if ((TileType)in == Paddle) mPaddlePos = mPos;
          }
          else mTiles[mPos] = (TileType)in;
        }
        mIndex = 0;
      }
      break;
      case 2: mPos.y = in; break;
      case 1: mPos.x = in; break;
    }
  }
  
  int out()
  {
    if (mBallPos.x == mPaddlePos.x) return 0;
    return mBallPos.x - mPaddlePos.x < 0 ? -1 : 1;
  }
  
  int getScore() const { return mScore; }
  
  void printToFile()
  {
    int w = 0; int h = 0;
    for (auto& tile : mTiles)
    {
      w = std::max(w, tile.first.x);
      h = std::max(h, tile.first.y);
    }
    ++w; ++h;
    Image img(w, h);
    for (auto& tile : mTiles)
    {
      char v = ' ';
      switch (tile.second) {
        case Empty: v = ' '; break;
        case Wall: v = '#'; break;
        case Block: v = '+'; break;
        case Paddle: v = '_'; break;
        case Ball: v = 'o'; break;
      }
      img.set(tile.first.x, tile.first.y, v);
    }
    img.saveToFile("output2.txt");
  }
  
private:
  int mIndex = 0;
  Vec2 mPos;
  std::unordered_map<Vec2, TileType> mTiles;
  int mScore = 0;
  Vec2 mBallPos;
  Vec2 mPaddlePos;
};

std::unique_ptr<Input> getInput()
{
  std::vector<LONG> rawInput;
  std::ifstream file("part2.txt", std::ios::in);
  std::string line;
  while (std::getline(file, line, ','))
  {
    LONG instruction = std::stoll(line);
    rawInput.push_back(instruction);
  }
  auto result = std::make_unique<Input>(rawInput);
  return result;
}

inline LONG arOp(LONG x, LONG y, LONG c) { return c == 1 ? x + y : x * y; }
inline bool jmpOp(LONG x, LONG c)
{
  return (c == 5) ? (x != 0) : (x == 0);
}
inline int compOp(LONG x, LONG y, LONG c)
{
  return static_cast<int>(((c == 7) ? (x < y) : (x == y)));
}

void compute(Input& input, Cabinet& cabinet)
{
  LONG relativeBase = 0;
  unsigned int i = 0;
  while (1)
  {
    const auto inst = input[i];
    if (inst == 99) return;
    const auto op = inst % 10;
    if (op == 3 || op == 4 || op == 9) // mem op or relateve base change.
    {
      const int mode = inst / 100;
      const LONG s = input[i + 1];
      const LONG offset = mode == 2 ? relativeBase : 0;
      if (op == 4)
      {
        const LONG value = (mode % 2) ? s : input[s + offset];
        cabinet.in(value);
      }
      else if (op == 3)
        input[s + offset] = cabinet.out();
      else relativeBase += ((mode % 2) ? s : input[s + offset]); // relative base change op.
      i += 2;
    }
    else
    {
      const LONG v0 = input[i + 1];
      const LONG v1 = input[i + 2];
      const LONG xMode = ((inst / 100) % 10);
      const LONG yMode = ((inst / 1000) % 10);
      const LONG x = (xMode % 2) ? v0 : input[v0 + (!xMode ? 0 : relativeBase)];
      const LONG y = (yMode % 2) ? v1 : input[v1 + (!yMode ? 0 : relativeBase)];
      const int outputMode = inst / 10000;
      const LONG outputOffset = outputMode == 2 ? relativeBase : 0;
      if (op == 1 || op == 2) // arith op
      {
        input[input[i + 3] + outputOffset] = arOp(x, y, op);
        i += 4;
      }
      else if (op == 5 || op == 6) // jmp
        i = jmpOp(x, op) ? y : i + 3;
      else // comp op
      {
        input[input[i + 3] + outputOffset] = compOp(x, y, op);
        i += 4;
      }
    }
  }
}

int main()
{
  auto origInput = getInput();
  MEASURE(
  {
    auto input = std::make_unique<Input>(origInput->getMain());
    (*input)[0] = 2;
    Cabinet cabinet;
    compute(*input, cabinet);
#if PRINT_OUTPUT
    std::cout << cabinet.getScore() << "\n";
    cabinet.printToFile();
#endif
  }, 1, 1);
}
