# Define compiler and flags (adjust as needed)
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

# Define all source files
SRC_FILES = ImageSampler.cpp

# Define the object files (automatically generated from source files)
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Define the executable name
TARGET = image_sampler

# Define libraries to link with (adjust as needed)
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui 

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
  $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_FILES) $(LIBS)

$(OBJ_FILES): $(SRC_FILES) ConfidenceMetrics.h
  $(CXX) $(CXXFLAGS) -c $(SRC_FILES) -I. -MMD  # Generate dependency info

clean:
  rm -rf $(OBJ_FILES) $(TARGET)

-include $(shell mkdir -p .deps && find . -name "*.cpp" -exec touch -d {} \; 2> /dev/null)  # Track dependencies automatically
