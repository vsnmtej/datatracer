cmake_minimum_required(VERSION 3.0)
project(TF_Check)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find TensorFlow
find_package(TensorFlow REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Add executable
add_executable(hand_gesture_recognition main.cpp)

# Include TensorFlow headers
target_include_directories(hand_gesture_recognition PRIVATE ${TensorFlow_INCLUDE_DIRS})

# Link TensorFlow and OpenCV libraries
target_link_libraries(TF_Check PRIVATE ${TensorFlow_LIBRARIES} ${OpenCV_LIBRARIES})
