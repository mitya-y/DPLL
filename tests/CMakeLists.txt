cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG "main"
  SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/googletest"
)
FetchContent_MakeAvailable(googletest)

enable_testing()

target_sources(${CMAKE_PROJECT_NAME} PRIVATE test_loading.cpp test_algorithm.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} gtest_main gmock_main)

execute_process(COMMAND bash -c "cp -r ${CMAKE_SOURCE_DIR}/tests/files ${CMAKE_SOURCE_DIR}/build")
