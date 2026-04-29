cmake_minimum_required(VERSION 3.10)

project(TimetableScheduler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(CORE_SOURCES
    src/scheduler_api.cpp      # <--- ADD IT HERE
    src/parser/csv_parser.cpp
    src/parser/db_interface.cpp
    src/constraints/constraint_manager.cpp
    src/solver/solver.cpp
    src/output/heuristics.cpp
    src/output/formatter.cpp
    src/output/excel_exporter.cpp
)

add_executable(TimetableScheduler src/main.cpp ${CORE_SOURCES})

# Notice we no longer explictly list src/scheduler_api.cpp here, since it's in CORE_SOURCES now
add_library(scheduler SHARED ${CORE_SOURCES})

target_link_libraries(TimetableScheduler xlsxwriter z sqlite3)
target_link_libraries(scheduler xlsxwriter z sqlite3)