cmake_minimum_required(VERSION 3.20)
project(SMCP ASM C CXX)

set(PLATFORM "mcu" CACHE STRING "Target platform (mcu, host)")
set_property(CACHE PLATFORM PROPERTY STRINGS "mcu" "host")

option(USE_MODERN_CPP "Use modern C++ syntax" ON)

message(STATUS "Building for platform: ${PLATFORM}")

if(PLATFORM STREQUAL "mcu")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchains/mcu.cmake")
elseif(PLATFORM STREQUAL "host") 
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchains/host.cmake")
else()
    message(FATAL_ERROR "Unknown platform: ${PLATFORM}")
endif()

include(${CMAKE_TOOLCHAIN_FILE})

if(PLATFORM STREQUAL "mcu")
    add_subdirectory(platforms/mcu/bare)
elseif(PLATFORM STREQUAL "host") 
    add_subdirectory(platforms/host)
endif()