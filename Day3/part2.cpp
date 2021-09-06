#include <fstream>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../common.hpp"

struct Point
{
  int x; int y;
};

using Wire = std::vector<Point>;
using Wires = std::pair<Wire, Wire>;

Wires getInput()
{
  Wires result = { {}, {} };
  std::ifstream file("part2.txt", std::ios::in);
  for (int i = 0; i < 2; ++i)
  {
    auto& wire = i == 0 ? result.first : result.second;
    Point pt; pt.x = 0; pt.y = 0;
    wire.push_back(pt);
    std::string wireStr; std::getline(file, wireStr);
    std::istringstream wireStrS(wireStr); std::string inst;
    while (std::getline(wireStrS, inst, ','))
    {
      const char dir = inst[0];
      int amount = std::stoi(std::string(inst.begin() + 1, inst.end()));
      const bool R = dir == 'R'; const bool L = dir == 'L';
      const bool U = dir == 'U'; const bool D = dir == 'D';
      if (R || L) pt.x += R ? amount : -amount;
      if (U || D) pt.y += U ? amount : -amount;
      wire.push_back(pt);
    }
  }
  return result;
}

inline unsigned int manhO(int x, int y) { return std::abs(x) + std::abs(y); }

bool intersect(int p0x, int p0y, int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int& u, int& v)
{
  // v = vertical, h = horizontal.
  const bool l0V = p0x == p1x;
  const bool l1V = p2x == p3x;
  if (!(l0V ^ l1V)) return false; // If both v or both h don't intersect.
  if (l0V) // swap v and h.
  {
    std::swap(p0x, p2x); std::swap(p0y, p2y);
    std::swap(p1x, p3x); std::swap(p1y, p3y);
  }
  if (p0x > p1x) std::swap(p0x, p1x);
  if (p2y > p3y) std::swap(p2y, p3y);
  if (p2x < p0x || p2x > p1x) return false;
  if (p2y <= p0y && p3y >= p0y && manhO(p2x, p0y) != 0)
  {
    u = p2x; v = p0y;
    return true;
  }
  return false;
}

unsigned int solve(const Wires& wires)
{
  unsigned int s = -1; int p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, u, v;
  unsigned int s0 = 0;
  for (int i = 0; i < wires.second.size() - 1; ++i)
  {
    p0x = wires.second[i].x;     p0y = wires.second[i].y;
    p1x = wires.second[i + 1].x; p1y = wires.second[i + 1].y;
    unsigned int s1 = 0;
    for (int j = 0; j < wires.first.size() - 1; ++j)
    {
      p2x = wires.first[j].x;     p2y = wires.first[j].y;
      p3x = wires.first[j + 1].x; p3y = wires.first[j + 1].y;
      if (intersect(p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, u, v))
      {
        unsigned int d = std::abs(u - p0x) + std::abs(u - p2x) + std::abs(v - p0y) + std::abs(v - p2y);
        s = std::min(s, s0 + s1 + d);
      }
      s1 += std::abs(p2x - p3x) + std::abs(p2y - p3y);
    }
    s0 += std::abs(p0x - p1x) + std::abs(p0y - p1y);
  }
  return s;
}

int main()
{
  auto wires = getInput();
  unsigned int d;
  MEASURE(
  d = solve(wires);
  , 1);
  std::cout << "result : " << d << std::endl;
}
