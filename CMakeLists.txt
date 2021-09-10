cmake_minimum_required(VERSION 2.8)

project (AOC2019)

set(CMAKE_CXX_STANDARD 17)

set(Days
	Day1
	Day2
	Day3
	Day4
	Day5
	Day7
	Day9
	Day11
	Day13
	Day14
)

foreach (Day ${Days})
	add_executable (${Day}.part1
		${Day}/part1.cpp
		common.hpp
	)
	add_executable (${Day}.part2
		${Day}/part2.cpp
		common.hpp
	)
endforeach(Day)

# SFML

set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE INTERNAL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE INTERNAL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE INTERNAL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SFML)

set(ExtendedDays
	Day14
)
set(Parts
	part1
	part2
)

foreach (day ${ExtendedDays})
	foreach (part ${Parts})
		target_link_libraries(${day}.${part} sfml-graphics)
		target_link_libraries(${day}.${part} sfml-window)
		target_link_libraries(${day}.${part} sfml-system)
	endforeach(part)
endforeach(day)
