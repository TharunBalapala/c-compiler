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

# CPack configuration for Debian packaging
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "minic")
set(CPACK_PACKAGE_VENDOR "MiniC Team")
set(CPACK_PACKAGE_CONTACT "MiniC Developers <dev@example.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A minimal subset C compiler producing x86-32 assembly.")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

# Debian specific dependencies
# Since the compiler generates x86-32 assembly (-m32), we need gcc-multilib and 32-bit headers.
set(CPACK_DEBIAN_PACKAGE_DEPENDS "gcc, gcc-multilib, libc6-dev-i386")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
