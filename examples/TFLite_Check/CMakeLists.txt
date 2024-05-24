cmake_minimum_required(VERSION 3.17)
project(TFLiteCheck)

set(CMAKE_CXX_STANDARD 14)
if(APPLE)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/install/lib/cmake/opencv4")
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")

# Check if CMAKE_SOURCE_DIR contains "example"
string(FIND "${CMAKE_SOURCE_DIR}" "/example" EXAMPLE_FOUND)
if(EXAMPLE_FOUND GREATER -1)
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../)
else()
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
endif()

if(APPLE)
find_package(Datasketches  REQUIRED)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/include/)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/libs/)

INCLUDE_DIRECTORIES(/Users/venkatapydialli/Projects/Edgetpu/datatracer/include/)
INCLUDE_DIRECTORIES(/Users/venkatapydialli/Projects/Edgetpu/datatracer/build/)

# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES($(CMAKE_CURRENT_SOURCE_DIR)/../tflite-dist/libs/)
else()
message(STATUS "NOT apple: ${OpenCV_DIR}")
INCLUDE_DIRECTORIES(${SOURCE_DIR}/include/datasketches)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/build)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tensorflow_src/)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tflite_v2/flatbuffers/include/)
endif()
add_executable(${PROJECT_NAME} main.cpp)

if(APPLE)
    ADD_LIBRARY(tensorflowlite SHARED IMPORTED)
    set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/../tflite-dist/libs/mac/libtensorflowlite.dylib)

    ADD_LIBRARY(imagesampler SHARED IMPORTED)
    set_property(TARGET imagesampler PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/build/libimagesampler.dylib)

    ADD_LIBRARY(imageprofiler SHARED IMPORTED)
    set_property(TARGET imageprofiler PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/build/libimageprofiler.dylib)
else()
    ADD_LIBRARY(imagesampler SHARED IMPORTED)
    set_property(TARGET imagesampler PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/build/libimagesampler.so)

    ADD_LIBRARY(imageprofiler SHARED IMPORTED)
    set_property(TARGET imageprofiler PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/build/libimageprofiler.so)

    ADD_LIBRARY(tensorflowlite SHARED IMPORTED)
    set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION ${SOURCE_DIR}/../tflite_v2/libtensorflowlite.so)
endif()
    target_link_libraries(TFLiteCheck tensorflowlite ${OpenCV_LIBS} imagesampler imageprofiler)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/libs/windows_x64/tensorflowlite.dll ${CMAKE_BINARY_DIR}/Release/
    )

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${OpenCV_DIR}/x64/vc15/bin/opencv_world452.dll ${CMAKE_BINARY_DIR}/Release/
    )
endif()
