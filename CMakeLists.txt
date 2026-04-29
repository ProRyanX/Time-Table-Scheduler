cmake_minimum_required(VERSION 3.10)

project(TimetableScheduler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/parser/csv_parser.cpp
    src/constraints/constraint_manager.cpp
    src/solver/solver.cpp
    src/output/heuristics.cpp
    src/output/formatter.cpp
)

add_executable(TimetableScheduler ${SOURCES})