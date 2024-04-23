cmake_minimum_required(VERSION 3.17)

add_library(core_helpers SHARED imghelpers.cpp)

# Include Directories
target_include_directories(core_helpers PUBLIC ${CMAKE_SOURCE_DIR}/include)
