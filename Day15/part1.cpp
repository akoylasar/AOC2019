#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <array>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#define GLAD_GL_IMPLEMENTATION
#include "gl.h"

#include "../common.hpp"

#define VISUALISE_WALLS 0
#define AUTOMATED 1

const int kNumDefaultTiles = 20;
const int kDefaultScreenRes = 1700;

using LONG = long long;
static_assert(sizeof(LONG) >= 8, "LONG not supported.");
using RawInput = std::vector<LONG>;
using Output = std::vector<LONG>;

class Input
{
public:
  Input(RawInput&& rawInput) : mMain(rawInput)
  {}
  LONG& operator[](LONG adr)
  {
    if (adr < mMain.size()) return mMain[adr];
    return mExtraMem[adr];
  }
private:
  RawInput mMain;
  std::map<LONG, LONG> mExtraMem;
};

inline LONG arOp(LONG x, LONG y, LONG c) { return c == 1 ? x + y : x * y; }

inline bool jmpOp(LONG x, LONG c)
{
  return (c == 5) ? (x != 0) : (x == 0);
}

inline int compOp(LONG x, LONG y, LONG c)
{
  return static_cast<int>(((c == 7) ? (x < y) : (x == y)));
}

// https://en.wikipedia.org/wiki/Pairing_function
// accepting collisions mirrored pairs on each quadrant.
std::size_t FouadHash(const sf::Vector2i& v)
{
  int n = v.x + v.y;
  return (n * (n + 1) / 2) + v.y;
}
namespace std
{
  template<> struct hash<sf::Vector2i>
  {
    std::size_t operator()(const sf::Vector2i& v) const noexcept { return FouadHash(v); }
  };
}
using TileMap = std::unordered_map<sf::Vector2i, int>;
void renderTileMap()
{
  
}
class TileMapRenderer : public sf::Drawable, public sf::Transformable
{
public:
  TileMapRenderer() = default;
  int findExtent(const TileMap& tileMap)
  {
    int extent = 0;
    for (const auto& tile : tileMap)
    {
      extent = std::max(std::abs(tile.first.x), extent);
      extent = std::max(std::abs(tile.first.y), extent);
    }
    return extent * 2;
  }
  
  void prepareForDraw(const TileMap& tileMap,
                      const sf::Vector2i& robotPos,
                      int numTiles = kNumDefaultTiles,
                      const int screenRes = kDefaultScreenRes)
  {
    mVao.setPrimitiveType(sf::Quads);
    const int extent = findExtent(tileMap);
    numTiles = std::max(extent + 1, numTiles);
    mVao.resize(numTiles * numTiles * 4);
    const int tileRes = screenRes / numTiles;
    for (int j = 0; j < numTiles; ++j)
    {
      for (int i = 0; i < numTiles; ++i)
      {
        sf::Vertex* quad = &mVao[(i + numTiles * j) * 4];

        const sf::Vector2i pos{i - numTiles / 2, numTiles / 2 - j};
        sf::Color col = sf::Color{127, 127, 127};
        const float margin = pos == robotPos ? tileRes / 10.0f : tileRes / 20.0f;

        const auto it = tileMap.find(pos);
        if (it != tileMap.end())
        {
          if (it->second == 1) col = sf::Color::White; // on free tile.
          else if (it->second == 2) col = sf::Color::Red; // on oxygen system.
#if(VISUALISE_WALLS)
          else if (it->second == -1) col = sf::Color{150, 75, 0}; // wall brown.
#endif
        }
        for (int k = 0; k < 4; ++k) quad[k].color = col;

        quad[0].position = sf::Vector2f(i * tileRes + margin, j * tileRes + margin);
        quad[1].position = sf::Vector2f((i + 1) * tileRes - margin, j * tileRes + margin);
        quad[2].position = sf::Vector2f((i + 1) * tileRes - margin, (j + 1) * tileRes - margin);
        quad[3].position = sf::Vector2f(i * tileRes + margin, (j + 1) * tileRes - margin);
      }
    }
  }
  
  void draw(sf::RenderTarget& target,
            sf::RenderStates states) const override
  {
    states.transform *= getTransform();
    target.draw(mVao, states);
  }
  
private:
  sf::VertexArray mVao;
};

class Robot
{
public:
  Robot() : mTileMap()
  {
    std::vector<LONG> rawInput;
    std::ifstream file("part1.txt", std::ios::in);
    DEBUG_ASSERT_MSG(file.is_open(), "Failed to open input file.");
    std::string line;
    while (std::getline(file, line, ','))
    {
      LONG instruction = std::stoll(line);
      rawInput.push_back(instruction);
    }
    mInput = std::make_unique<Input>(std::move(rawInput));
    mTileMap.insert({sf::Vector2i{0, 0}, 1});
  }
  
  const sf::Vector2i& getPos() const
  {
    return mPos;
  }
  
  const TileMap& getTileMap() const
  {
    return mTileMap;
  }
  
  // 0 hit a wall.
  // 1 moved one step in the requested direction.
  // 2 moved one step and has found oxygen system.
  int move(int dir)
  {
    while (true)
    {
      const auto inst = (*mInput)[mInstructionPtr];
      if (inst == 99) return true;
      const auto op = inst % 10;
      if (op == 3 || op == 4 || op == 9) // mem op or relateve base change.
      {
        const int mode = inst / 100;
        const LONG s = (*mInput)[mInstructionPtr + 1];
        const LONG offset = mode == 2 ? mRelativeBase : 0;
        if (op == 4)
        {
          mInstructionPtr += 2;
          
          const LONG output = (mode % 2) ? s : (*mInput)[s + offset];
          // wait for the repair droid to finish the movement operation and
          // report on the status of the repair droid via an output instruction.
          return update(dir, output);
        }
        // accept a movement command via an input instruction and
        // aend the movement command to the repair droid.
        else if (op == 3)
          (*mInput)[s + offset] = dir;
        else mRelativeBase += ((mode % 2) ? s : (*mInput)[s + offset]); // relative base change op.
        mInstructionPtr += 2;
      }
      else
      {
        const LONG v0 = (*mInput)[mInstructionPtr + 1];
        const LONG v1 = (*mInput)[mInstructionPtr + 2];
        const LONG xMode = ((inst / 100) % 10);
        const LONG yMode = ((inst / 1000) % 10);
        const LONG x = (xMode % 2) ? v0 : (*mInput)[v0 + (!xMode ? 0 : mRelativeBase)];
        const LONG y = (yMode % 2) ? v1 : (*mInput)[v1 + (!yMode ? 0 : mRelativeBase)];
        const int outputMode = inst / 10000;
        const LONG outputOffset = outputMode == 2 ? mRelativeBase : 0;
        if (op == 1 || op == 2) // arith op
        {
          (*mInput)[(*mInput)[mInstructionPtr + 3] + outputOffset] = arOp(x, y, op);
          mInstructionPtr += 4;
        }
        else if (op == 5 || op == 6) // jmp
          mInstructionPtr = jmpOp(x, op) ? y : mInstructionPtr + 3;
        else // comp op
        {
          (*mInput)[(*mInput)[mInstructionPtr + 3] + outputOffset] = compOp(x, y, op);
          mInstructionPtr += 4;
        }
      }
    }
  }
  
private:
  int update(int direction, int output)
  {
    sf::Vector2i wallPos{mPos};
    auto& posToUpdata = output ? mPos : wallPos;
    
    // 1 == north, 2 == south, 3 == west, 4 == east
    if (direction == 1) posToUpdata.y += 1;
    else if (direction == 2) posToUpdata.y -= 1;
    else if (direction == 3) posToUpdata.x -= 1;
    else posToUpdata.x += 1;
    mTileMap[posToUpdata] = output ? output : -1;
    
    return output;
  }
  
private:
  // intcode computer related
  std::unique_ptr<Input> mInput;
  unsigned int mInstructionPtr = 0;
  LONG mRelativeBase = 0;

  // robot and tile related
  sf::Vector2i mPos = {0, 0}; // in cartesian coordinates.
  TileMap mTileMap;
};

class RobotController
{
public:
  RobotController(Robot& robot) : mRobot(robot) {}
  const int getMoves() const {return mTileRec.size();}
  const bool isFinished() const {return mFinished;};
  bool update()
  {
    const sf::Vector2i curPos = mRobot.getPos();
    const TileMap& tileMap = mRobot.getTileMap();
    // check if we found the final spot if so we halt.
    auto it = tileMap.find(curPos);
    if (it != tileMap.end() && it->second == 2)
    {
      mFinished = true;
      return false;
    }
    const std::array<sf::Vector2i, 4> allNeighbours
    {
     sf::Vector2i{curPos.x - 1, curPos.y},
     sf::Vector2i{curPos.x + 1, curPos.y},
     sf::Vector2i{curPos.x, curPos.y - 1},
     sf::Vector2i{curPos.x, curPos.y + 1}
    };
    
    std::vector<sf::Vector2i> toExplore;
    for (const auto& neighbour : allNeighbours)
    {
      auto it = tileMap.find(neighbour);
      // we don't want to visit a tile that we've already visited.
      if (it == tileMap.end())
      {
        if (!mTileRec.empty() && mTileRec.top() == neighbour) continue; // skip tile from which we came.
        toExplore.push_back(neighbour);
      }
    }

    for (const auto& neighbour : toExplore)
    {
      const int dir = getMoveDirection(curPos, neighbour);
      if (mRobot.move(dir))
      {
        mTileRec.push(curPos);
        return true;
      }
    }
    // none of the neighbours were available so backtrack.
    if (!mTileRec.empty())
    {
      const sf::Vector2i prevTile = mTileRec.top();
      mTileRec.pop();
      const int dir = getMoveDirection(curPos, prevTile);
      DEBUG_ASSERT_MSG(mRobot.move(dir), "Something's screwed up, previous tile should always be available.");
      return true;
    }
    DEBUG_ASSERT_MSG(false, "Back to origin and everything already checked. Something is wrong.");
  }
private:
  int getMoveDirection(const sf::Vector2i& start, const sf::Vector2i& dest)
  {
    // up(1), down(2), left(3), right(4)
    int result = 0;
    if (dest.x < start.x) result = 3;
    else if (dest.x > start.x) result = 4;
    else if (dest.y < start.y) result = 2;
    else if (dest.y > start.y) result = 1;
    DEBUG_ASSERT_MSG(result, "directiont can be only one of 1, 2, 3, 4.");
    return result;
  }
  
private:
  Robot& mRobot;
  std::stack<sf::Vector2i> mTileRec;
  bool mFinished = false;
};

int main()
{
  sf::Uint32 windowStyle = sf::Style::Titlebar | sf::Style::Close;
  sf::RenderWindow window(sf::VideoMode(kDefaultScreenRes, kDefaultScreenRes), "Day 14 part 1", windowStyle);
  window.setVerticalSyncEnabled(true);
  
  Robot robot;
  TileMapRenderer tileMap;
  tileMap.prepareForDraw(robot.getTileMap(), robot.getPos());
#if (AUTOMATED)
  RobotController controller{robot};
#else
  window.setKeyRepeatEnabled(true);
#endif
  sf::Clock clock;
  constexpr int kDelay = 25;
  while (window.isOpen())
  {
    sf::Event event;
    int moveSucceeded = false;
#if (AUTOMATED)
    if (controller.isFinished())
    {
      if (clock.getElapsedTime().asMilliseconds() > 2000)
      {
        std::cout << "Robot at position after " << controller.getMoves() << " moves\n";
        return 0;
      }
    }
		else
    {
      if (clock.getElapsedTime().asMilliseconds() > kDelay)
      {
        moveSucceeded = controller.update();
        clock.restart();
      }
    }
    if (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }
#else
    if (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed)
      {
        switch (event.key.code)
        {
          case sf::Keyboard::Up:
            moveSucceeded = robot.move(1); break;
          case sf::Keyboard::Down:
            moveSucceeded = robot.move(2); break;
          case sf::Keyboard::Left:
            moveSucceeded = robot.move(3); break;
          case sf::Keyboard::Right:
            moveSucceeded = robot.move(4); break;
          default: break;
        }
      }
    }
#endif
    if (moveSucceeded)
      tileMap.prepareForDraw(robot.getTileMap(), robot.getPos());
    window.clear();
    window.draw(tileMap);
    window.display();
  }
  return 0;
}
