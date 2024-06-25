# DataTracer

Datatracer is a C++ library designed for AI model observability and sampling in real-time, specifically focused on edge devices. Manage, debug, and retrain AI models at the edge efficiently.
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

## Table of Contents

- [Installation](#installation)
  - [From Source](#from-source)
  - [Prebuilt Packages](#prebuilt-packages)
  - [Docker Images](#docker-images)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Installation

### From Source

#### Prerequisites

- C++17 or higher
- CMake 3.10 or higher

#### Steps

1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/datatracer.git
   cd datatracer
   mkdir build
   cd build
   ##### Release cmake ..
   make
  ##### Debug
sudo make install


2. Prebuilt Packages
You can download and install prebuilt packages for various platforms:

Debian: Download NetraAI-v1.0.0-win64.zip
Raspian: Download NetraAI-v1.0.0-linux.tar.gz
Ubuntu: Download NetraAI-v1.0.0-macos.zip

Installation Instructions

For Linux:
tar -xzf NetraAI-v1.0.0-linux.tar.gz
sudo cp -r NetraAI /usr/local/

3.Docker Images

docker pull yourusername/netraai:latest
docker run -it --rm yourusername/datatracer

You can also use Docker Compose to set up a more complex environment. Here’s an example docker-compose.yml file:
version: '3.8'
services:
  netraai:
    image: yourusername/netraai:latest
    volumes:
      - ./your-local-dir:/app
    ports:
      - "8080:8080"


docker-compose up
## Usage

Basic Example
Here’s a simple example to get you started:


## API Reference:

Link to doxygen

## Contributors

##License

##Contact
