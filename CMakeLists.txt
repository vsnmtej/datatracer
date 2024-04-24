cmake_minimum_required(VERSION 3.17)
project(datum_logs)
set(CMAKE_CXX_STANDARD 14)

# Try to find OpenCV
# set(OpenCV_DIR ....)
set(OpenCV_DIR "/usr")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(DataSketches_DIR "/usr/local/")
find_package(DataSketches 5.1 REQUIRED)
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/libs/)

# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES($(CMAKE_CURRENT_SOURCE_DIR)/../tflite-dist/libs/)

ADD_LIBRARY(tensorflowlite SHARED IMPORTED)
set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/libs/mac/libtensorflowlite.dylib)

# lib has 1 file: libtensorflowlite.so

#Add Subdirectories
add_subdirectory(src)
#add_subdirectory(src/src)
#add_subdirectory(src/test)
#add_subdirectory(src/utils)

#add_subdirectory(src/src/helpers)
#add_subdirectory(src/src/sampling)
#add_subdirectory(src/src/sketches)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/src)

# file(GLOB SOURCE_FILES src/*.cpp)
# add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

add_library(datum_logs SHARED
            src/src/datatracer.cpp
            src/src/helpers/imghelpers.cpp
            src/src/sampling/ImageSampler.cpp
            src/src/sketches/kll.cpp)

#target_link_libraries(datum_logs core::core)
#target_link_libraries(my_library core::helpers::helpers)
#target_link_libraries(my_library core::sampling::sampling)
#target_link_libraries(my_library core::sketches::sketches)
target_link_libraries(${CMAKE_PROJECT_NAME} tensorflowlite ${OpenCV_LIBS} datasketches pthread)

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
