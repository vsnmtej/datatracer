cmake_minimum_required(VERSION 3.17)
project(imagesampler)

# Try to find OpenCV
set(OpenCV_DIR "/usr")
find_package(OpenCV REQUIRED)

# Try to find DataSketches
set(DataSketches_DIR "/usr/local/")
find_package(DataSketches 5.1 REQUIRED)

#find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
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

add_executable(ImageProcessingTest
                src/helpers/imghelpers.cpp
                src/helpers/tests/ImageProcessingTest.cpp
              )

add_executable(IniParserTest
                src/helpers/IniParser.cpp
                src/helpers/tests/IniParserTest.cpp
              )

#add_executable(SaverTest
#                src/helpers/saver.cpp
#                src/helpers/tests/SaverTest.cpp
#              )

#add_executable(image_profiler_test
#                src/helpers/saver.cpp
#                src/helpers/IniParser.cpp
#                src/helpers/imghelpers.cpp
#                src/profiles/imageprofile.cpp
#                src/profiles/tests/image_profiler_test.cpp
#              )

#add_executable(test_image_sampler
#                src/helpers/saver.cpp
#                src/helpers/IniParser.cpp
#                src/helpers/imghelpers.cpp
#                src/sampling/ImageSampler.cpp
#                src/sampling/tests/test_image_sampler.cpp
#              )

target_link_libraries(ImageProcessingTest gtest gtest_main ${OpenCV_LIBS} pthread)
target_link_libraries(IniParserTest gtest gtest_main ${OpenCV_LIBS} pthread)
#target_link_libraries(SaverTest gtest gtest_main ${OpenCV_LIBS} pthread)
#target_link_libraries(image_profiler_test gtest gtest_main ${OpenCV_LIBS} pthread)
#target_link_libraries(test_image_sampler gtest gtest_main ${OpenCV_LIBS} pthread)

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

enable_testing()
#Test
add_test(NAME ImageProcessingTest COMMAND ImageProcessingTest)
add_test(NAME IniParserTest COMMAND IniParserTest)
