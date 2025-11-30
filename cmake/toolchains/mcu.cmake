set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# TODO: experiment with flags
# set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -specs=nosys.specs -ffunction-sections -fdata-sections")
set(COMMON_FLAGS "-mcpu=cortex-m3 -O2 -mthumb -specs=nosys.specs")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COMMON_FLAGS "${COMMON_FLAGS} -DDEBUG")
else()
    set(COMMON_FLAGS "${COMMON_FLAGS}")
endif()

#set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
#set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++17 -fno-rtti -fno-exceptions")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m3 -mthumb")
# set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "ASM Flags: ${CMAKE_ASM_FLAGS}") 
message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")