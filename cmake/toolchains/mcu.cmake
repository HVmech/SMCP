set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb")
set(COMMON_FLAGS "${COMMON_FLAGS} -specs=nosys.specs")          # Заглушки для syscall
set(COMMON_FLAGS "${COMMON_FLAGS} -ffunction-sections")         # Каждая функция в отдельной секции
set(COMMON_FLAGS "${COMMON_FLAGS} -fdata-sections")

set(COMMON_FLAGS "${COMMON_FLAGS} -fno-exceptions")             # Отключить исключения C++
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-rtti")                   # Отключить RTTI
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-unwind-tables")          # Отключить EH-таблицы
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-asynchronous-unwind-tables")
set(COMMON_FLAGS "${COMMON_FLAGS} -fno-threadsafe-statics")     # Отключить потокобезопасную инициализацию статиков


# TODO: experiment with flags
# set(COMMON_FLAGS "-mcpu=cortex-m3 -mthumb -specs=nosys.specs -ffunction-sections -fdata-sections")
##set(COMMON_FLAGS "-mcpu=cortex-m3 -O2 -mthumb -specs=nosys.specs")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COMMON_FLAGS "${COMMON_FLAGS} -DDEBUG")
else()
    set(COMMON_FLAGS "${COMMON_FLAGS}")
endif()

#set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
#set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++17 -fno-rtti -fno-exceptions")
##set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}")
##set(CMAKE_ASM_FLAGS "-mcpu=cortex-m3 -mthumb")
# set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections")
##set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++17")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m3 -mthumb")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")                 # Удалить неиспользуемые секции

#message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
#message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
#message(STATUS "ASM Flags: ${CMAKE_ASM_FLAGS}") 
#message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "ASM Flags: ${CMAKE_ASM_FLAGS}") 
message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")