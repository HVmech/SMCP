cmake_minimum_required(VERSION 3.20)
project(stm32_blink ASM C CXX)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

option(USE_MODERN_CPP "Use modern C++ syntax with direct register access" OFF)

set(CMAKE_CXX_FLAGS "-mcpu=cortex-m3 -mthumb -O2 -specs=nosys.specs")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -T${CMAKE_SOURCE_DIR}/platforms/mcu/bare/linker.ld")

add_executable(${PROJECT_NAME}.elf 
    platforms/mcu/bare/startup.s
    platforms/mcu/bare/main.cpp
)

set_source_files_properties(platforms/mcu/bare/startup.s PROPERTIES LANGUAGE ASM)

if(USE_MODERN_CPP)
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE NUKE_SOKOL_ASS_WITH_MODERN_CPP)
    message(STATUS "Modern C++ syntax enabled")
else()
    message(STATUS "Modern C++ syntax disabled. Old C style activated")
endif()