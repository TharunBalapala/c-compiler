cmake_minimum_required(VERSION 3.10)

project(CCompiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add all source files here
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(ccompiler ${SRC_FILES})

# Include directories
target_include_directories(ccompiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable common warnings
if(MSVC)
  target_compile_options(ccompiler PRIVATE /W4)
else()
  target_compile_options(ccompiler PRIVATE -Wall -Wextra -pedantic)
endif()
