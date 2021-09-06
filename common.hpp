#pragma once

#include <chrono>
#include <iostream>

#define MEASURE(expression, numRuns)\
{\
using DDuration = std::chrono::duration<double, std::milli>;\
double duration = 0.0;\
for (int _run = 0; _run < numRuns; ++_run)\
{\
const auto _t0 = std::chrono::high_resolution_clock::now();\
{\
expression\
}\
const auto _t1 = std::chrono::high_resolution_clock::now();\
duration += std::chrono::duration_cast<DDuration>(_t1 - _t0).count();\
}\
const double avgDuration = duration / numRuns;\
std::cout << numRuns << " run(s) with with average run time of " << avgDuration << "(ms).\n";\
}
