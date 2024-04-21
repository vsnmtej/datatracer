cmake_minimum_required(VERSION 3.17)

add_subdirectory(helpers)
add_subdirectory(sampling)
add_subdirectory(sketches)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
