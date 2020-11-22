cmake_minimum_required(VERSION 2.8)

project (AOC2019)

set(CMAKE_CXX_STANDARD 17)

set (Days
	Day1
	Day2
	Day3
	Day4
	Day5
	Day7
	Day9
	Day11
	Day13
)

foreach (Day ${Days})
	add_executable (${Day}.part1
		${Day}/part1.cpp
	)
	add_executable (${Day}.part2
		${Day}/part2.cpp
	)
endforeach(Day)