cmake_minimum_required(VERSION 3.15)

project(optional)

set(CMAKE_CXX_STANDARD 20)

# Include directories for headers
include_directories(${CMAKE_SOURCE_DIR}/header)

# Add executable target
add_executable(optional 
    src/main.cpp
    header/optional.h

)
