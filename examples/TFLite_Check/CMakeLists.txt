cmake_minimum_required(VERSION 3.17)
project(TFLiteCheck)

set(CMAKE_CXX_STANDARD 14)
if(APPLE)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/install/lib/cmake/opencv4")
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")

if(APPLE)
find_package(Datasketches  REQUIRED)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/include/)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/libs/)

INCLUDE_DIRECTORIES(/Users/venkatapydialli/Projects/Edgetpu/datatracer/include/)
INCLUDE_DIRECTORIES(/Users/venkatapydialli/Projects/Edgetpu/datatracer/build/)

# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES($(CMAKE_CURRENT_SOURCE_DIR)/../tflite-dist/libs/)
endif()
add_executable(${PROJECT_NAME} main.cpp)

if(APPLE)
    ADD_LIBRARY(tensorflowlite SHARED IMPORTED)
    set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/libs/mac/libtensorflowlite.dylib)

    ADD_LIBRARY(imagesampler SHARED IMPORTED)
    set_property(TARGET imagesampler PROPERTY IMPORTED_LOCATION /Users/venkatapydialli/Projects/Edgetpu/datatracer/build/libimagesampler.dylib)

    ADD_LIBRARY(imageprofiler SHARED IMPORTED)
    set_property(TARGET imageprofiler PROPERTY IMPORTED_LOCATION /Users/venkatapydialli/Projects/Edgetpu/datatracer/build/libimageprofiler.dylib)

    target_link_libraries(TFLiteCheck tensorflowlite ${OpenCV_LIBS} imagesampler imageprofiler)
endif()

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
