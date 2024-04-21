cmake_minimum_required(VERSION 3.17)

add_library(core_helpers SHARED img_helper.cpp)

# Include Directories
target_include_directories(core_helpers ${CMAKE_SOURCE_DIR}/include)
