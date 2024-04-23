cmake_minimum_required(VERSION 3.17)

add_subdirectory(tests)
add_subdirectory(utils)
add_subdirectory(src)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
