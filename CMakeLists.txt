cmake_minimum_required(VERSION 3.16)
project(TheSalesmanProblem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(tsp_core
  src/graph.cpp
  src/io.cpp
  src/algorithms.cpp
  src/svg.cpp
)
target_include_directories(tsp_core PUBLIC src)

if(MSVC)
  target_compile_options(tsp_core PRIVATE /W4)
else()
  target_compile_options(tsp_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(tsp src/main.cpp)
target_link_libraries(tsp PRIVATE tsp_core)

include(CTest)
if(BUILD_TESTING)
  add_executable(tsp_tests tests/test_tsp.cpp)
  target_link_libraries(tsp_tests PRIVATE tsp_core)
  target_compile_definitions(tsp_tests PRIVATE TSP_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
  add_test(NAME unit_tests COMMAND tsp_tests)

  add_test(NAME cli_help COMMAND tsp --help)
  add_test(NAME cli_points COMMAND tsp ${CMAKE_CURRENT_SOURCE_DIR}/data/arr48.tsp)
  add_test(NAME cli_matrix COMMAND tsp ${CMAKE_CURRENT_SOURCE_DIR}/data/br17.atsp --start 3)
  add_test(NAME cli_bad_file COMMAND tsp does-not-exist.tsp)
  set_tests_properties(cli_bad_file PROPERTIES WILL_FAIL TRUE)
endif()
