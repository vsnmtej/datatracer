cmake_minimum_required(VERSION 3.17)

add_library(core_sketches SHARED kll.cpp)

# Include Directories
target_include_directories(core_sketches ${CMAKE_SOURCE_DIR}/include)
