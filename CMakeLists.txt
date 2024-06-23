cmake_minimum_required(VERSION 3.20)

# set(CMAKE_BUILD_TYPE Release)

project("dpll")

add_executable(${CMAKE_PROJECT_NAME} algorithm/dimacs_loader.cpp algorithm/dpll.cpp main.cpp)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 23)

enable_testing()
add_test(NAME test_algorithm
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test.py ${CMAKE_CURRENT_BINARY_DIR}/dpll ${CMAKE_SOURCE_DIR}/tests/files)
