cmake_minimum_required(VERSION 3.17)
project(datum_logs)
set(CMAKE_CXX_STANDARD 14)

# Try to find OpenCV
# set(OpenCV_DIR ....)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/build_opencv/")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Datasketches 3.2 REQUIRED)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/include/)
INCLUDE_DIRECTORIES(/tmp/install/DataSketches/libs/)

# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES($(CMAKE_CURRENT_SOURCE_DIR)/../tflite-dist/libs/)

ADD_LIBRARY(tensorflowlite SHARED IMPORTED)
set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/libs/mac/libtensorflowlite.dylib)

# lib has 1 file: libtensorflowlite.so

#Add Subdirectories
add_subdirectory(src)
#add_subdirectory(src/core)
#add_subdirectory(src/test)
#add_subdirectory(src/utils)

#add_subdirectory(src/core/helpers)
#add_subdirectory(src/core/sampling)
#add_subdirectory(src/core/sketches)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# file(GLOB SOURCE_FILES src/*.cpp)
# add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

add_library(datum_logs SHARED
            src/core/core.cpp
            src/core/helpers/image_helper.cpp
            src/core/sampling/uncertainity:sampling.cpp
            src/core/sketches/kll.cpp)

target_link_libraries(datum_logs core::core)
target_link_libraries(my_library core::helpers::helpers)
target_link_libraries(my_library core::sampling::sampling)
target_link_libraries(my_library core::sketches::sketches)
target_link_libraries(${CMAKE_PROJECT_NAME} tensorflowlite ${OpenCV_LIBS} datasketches pthread)

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
