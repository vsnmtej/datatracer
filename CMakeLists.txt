cmake_minimum_required(VERSION 3.17)
project(imagesampler)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INSTALL_PREFIX "install")

# Try to find OpenCV
# set(OpenCV_DIR ....)
set(OpenCV_DIR "/usr")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Try to find DataSketches
set(DataSketches_DIR "/usr/local/")
find_package(DataSketches 5.1 REQUIRED)
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/include/DataSketches)
INCLUDE_DIRECTORIES(/usr/local/libs/)

# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES($(CMAKE_CURRENT_SOURCE_DIR)/../tflite-dist/libs/)

#Add Subdirectories
#add_subdirectory(src)
#add_subdirectory(src/core/helpers)
#add_subdirectory(src/core/sampling)
#add_subdirectory(src/core/sketches)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(imagesampler SHARED
            src/core/sampling/ImageSampler.cpp
            src/core/helpers/imghelpers.cpp
			src/core/helpers/saver.cpp
			src/core/helpers/inireader.cpp
			)

add_library(imageprofiler SHARED
            src/core/helpers/imghelpers.cpp
			src/core/helpers/saver.cpp
			src/core/helpers/inireader.cpp
			src/core/profiles/imageprofile.cpp
			)

add_library(modelprofiler SHARED 
	    src/core/helpers/saver.cpp
            src/core/helpers/inireader.cpp
            src/core/profiles/modelprofile.cpp
            )	    
target_link_libraries(${CMAKE_PROJECT_NAME} ${OpenCV_LIBS} pthread)

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
