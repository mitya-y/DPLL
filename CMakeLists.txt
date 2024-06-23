cmake_minimum_required(VERSION 3.20)

# set(CMAKE_BUILD_TYPE Release)

project("dpll")

add_executable(${CMAKE_PROJECT_NAME} algorithm/dimacs_loader.cpp algorithm/dpll.cpp main.cpp)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 23)
