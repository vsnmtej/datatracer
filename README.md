# DataTracer

DataTracer is a project that includes three libraries: `libimageprofiler.so`,  `libimagesampler.so`, and `libmodelprofiler.so`.

This README provides instructions for setting up the environment, building the project, and compiling the libraries.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- libboost-all-dev
- libcurl4-openssl-dev
- libtar-dev
- libssl-dev
- libgtest-dev
- libopencv-dev

## Compilation

### Build libraries
#### Build with debug
```
cd build
cmake ..
make
```
#### Build for Release
```
cd build
cmake .. -D CMAKE_BUILD_TYPE=RELEASE
make
```

### Run test
```
cd build
cmake ..
make
make test
```

### Build Example
```
cd build
cmake ..
make
make TFLiteCheck
```

### Build using docker
```
cd Docker
./build.sh
```
