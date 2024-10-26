cmake_minimum_required(VERSION 3.5)

project(eevee_os_hardware)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

############################################################
# Create an executable
############################################################

############################################################
# Tests
############################################################
enable_testing()

add_executable(
    hdd_unittest
    test/hdd_unittest.cc
    src/hdd.c
)

target_link_libraries(
    hdd_unittest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hdd_unittest)
