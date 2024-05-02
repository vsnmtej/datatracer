cmake_minimum_required(VERSION 3.17)
project(imagesampler)

# Try to find OpenCV
set(OpenCV_DIR "/usr")
find_package(OpenCV REQUIRED)

# Try to find DataSketches
set(DataSketches_DIR "/usr/local/")
find_package(DataSketches 5.1 REQUIRED)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/include/DataSketches)
INCLUDE_DIRECTORIES(/usr/local/libs/)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(imagesampler SHARED
            src/helpers/saver.cpp
			src/helpers/IniParser.cpp
            src/helpers/imghelpers.cpp
            src/sampling/ImageSampler.cpp
			)

add_library(imageprofiler SHARED
			src/helpers/saver.cpp
			src/helpers/IniParser.cpp
            src/helpers/imghelpers.cpp
			src/profiles/imageprofile.cpp
			)

add_library(modelprofiler SHARED 
            src/helpers/saver.cpp
            src/helpers/IniParser.cpp
            src/profiles/modelprofile.cpp
            )	    

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
