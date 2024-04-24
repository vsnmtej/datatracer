cmake_minimum_required(VERSION 3.17)
project(image_sampler)

# Source files
set(SRC_FILES
    ImageSampler.cpp
)

# Executable
add_executable(image_sampler ${SRC_FILES})

# Include directories
target_include_directories(image_sampler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Libraries to link with
set(LIBS
    opencv_core
    opencv_imgproc
    opencv_highgui
)

target_link_libraries(image_sampler ${LIBS})

