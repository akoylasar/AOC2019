#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>
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

#define MEASURE_EXECUTION 0
#define PRINT_REGISTRATION 1

using LONG = long long;
static_assert(sizeof(LONG) >= 8, "LONG not supported.");
using RawInput = std::vector<LONG>;
using Output = std::vector<LONG>;

class Input
{
public:
  Input(const RawInput& rawInput) : mMain(rawInput)
  {}
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

class Robot
{
public:
  struct Vec2
  {
    int x = 0; int y = 0;
  };
  
  struct Tile
  {
    Vec2 pos;
    char color = 0;
    int paintCount = 0;
  };
  
  Robot()
  : mTiles()
  {
    mTiles.emplace_back();
    mCurrentTile = &mTiles.back();
    // Starts facing up.
    mDir.x = 0;
    mDir.y = 1;
    mCurrentTile->color = 1;
  }
  
  LONG getCurrentTileColor()
  {
    return mCurrentTile->color;
  }
  void command(char in)
  {
    ++mCmdTyp %= 2;
    if (mCmdTyp) // color
    {
      mCurrentTile->color = in;
      ++mCurrentTile->paintCount;
    }
    else // position
    {
      rot(in); // 0 = ccw, 1 = cw
      Tile t;
      t.pos.x = mCurrentTile->pos.x + mDir.x;
      t.pos.y = mCurrentTile->pos.y + mDir.y;
      auto it = std::find_if(mTiles.begin(), mTiles.end(), [&t](const Tile& s) { return s.pos.x == t.pos.x && s.pos.y == t.pos.y; });
      if (it != mTiles.end()) mCurrentTile = &(*it);
      else
      {
        mTiles.push_back(t);
        mCurrentTile = &mTiles.back();
      }
    }
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
      for (int i = mH - 1; i >= 0; --i)
      {
        for (int j = mW - 1; j >= 0; --j)
        {
          int idx = i * mW + j;
          file << (char)(mData[idx] ? 35 : 46);
        }
        file << "\n";
      }
    }
  private:
    std::vector<char> mData; int mW; int mH;
  };
  
  void printRegistration()
  {
    Vec2 l; Vec2 h;
    for (const auto& tile : mTiles)
    {
      l.x = std::min(tile.pos.x, l.x);
      l.y = std::min(tile.pos.y, l.y);
      h.x = std::max(tile.pos.x, h.x);
      h.y = std::max(tile.pos.y, h.y);
    }
    int width = h.x - l.x + 1;
    int height = h.y - l.y + 1;
    Vec2 offset;
    offset.x = std::abs(l.x);
    offset.y = std::abs(l.y);
    Image image(width, height);
    for (const auto& tile : mTiles)
      image.set(tile.pos.x + offset.x, tile.pos.y + offset.y ,tile.color);
    image.saveToFile("retistration.txt");
    std::cout << "Checkout the answer in the registration.txt file.\n";
  }
private:
  inline void rot(bool ccw)
  {
    // Opt: drop conditional.
    Vec2 res;
    res.x = ccw ? -mDir.y : mDir.y;
    res.y = ccw ? mDir.x : -mDir.x;
    mDir = res;
  }
  
  char mCmdTyp = 0;
  Vec2 mDir;
  Tile* mCurrentTile;
  std::vector<Tile> mTiles;
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

void compute(Input& input, Robot& robot)
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
        robot.command(value);
      }
      else if (op == 3) input[s + offset] = robot.getCurrentTileColor();
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
    Robot robot;
    compute(*input, robot);
#if PRINT_REGISTRATION && !MEASURE_EXECUTION
    robot.printRegistration();
#endif
  }
  , 1, 1);
}
