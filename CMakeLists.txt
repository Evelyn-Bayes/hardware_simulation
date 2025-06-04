cmake_minimum_required(VERSION 3.5)

project(eevee_os_hardware)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For test coverage analysis: enable debugging and turn off optimizations
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# *********************************************************************************************************************
# *                                                       TESTS                                                       *
# *********************************************************************************************************************

enable_testing()

add_executable(
    cpu_unittest
    test/cpu_unittest.cc
    src/cpu.c
    src/memory.c
)

target_link_libraries(
    cpu_unittest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(
    cpu_unittest
)
