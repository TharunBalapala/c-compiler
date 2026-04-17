cmake_minimum_required(VERSION 3.10)
project(minic VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(include)

# Create a library for the compiler backend
file(GLOB MINIC_SOURCES "lib/*.cpp")
add_library(minic_lib STATIC ${MINIC_SOURCES})

# Create the main executable
add_executable(minic src/main.cpp)
target_link_libraries(minic PRIVATE minic_lib)

# Installation instructions
install(TARGETS minic DESTINATION bin)
