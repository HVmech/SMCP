cmake_minimum_required(VERSION 3.20)
project(SMCP ASM C CXX)

set(DEFAULT_BUILD_TYPE "Debug")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, using default: ${DEFAULT_BUILD_TYPE}")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Build type" FORCE)
endif()

set(PLATFORM "mcu" CACHE STRING "Target platform (mcu, host)")
set_property(CACHE PLATFORM PROPERTY STRINGS "mcu" "host")

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