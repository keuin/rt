cmake_minimum_required(VERSION 3.0)
project(rt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(common_compiler_args "-Wall -Werror -Wno-unused -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${common_compiler_args} -g -ggdb -fsanitize=address -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${common_compiler_args} -O2")

set(CMAKE_VERBOSE_MAKEFILE on)

# main executable

add_executable(rt main.cpp vec.h bitmap.h ray.h bitfont.h hitlist.h object.h sphere.h viewport.h aa.h)
add_executable(image_output main_image_output.cpp vec.h bitmap.h bitfont.h hitlist.h object.h sphere.h viewport.h)
add_executable(simple_scanner main_simple_scanner.cpp vec.h bitmap.h ray.h timer.h bitfont.h hitlist.h object.h sphere.h viewport.h aa.h)

# googletest

add_executable(all_tests test_vec.cpp test_bitmap.cpp vec.h bitmap.h ray.h)

target_link_libraries(all_tests gtest_main)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

gtest_discover_tests(all_tests)