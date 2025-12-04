set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)

set(TOOLCHAIN_PREFIX arm-none-eabi)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

#set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_EXTENSIONS OFF)  # запретить GNU-расширения, если не нужны

# Флаги компиляции
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Og -g3 -Wall -Wextra -Werror -std=c11 -ffreestanding" CACHE STRING "")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g" CACHE STRING "")

# 🔑 Ключевое: флаги ЛИНКОВКИ (не C_FLAGS!)
set(CMAKE_EXE_LINKER_FLAGS 
    "-T ${CMAKE_SOURCE_DIR}/linker.ld ${CPU_FLAGS} -nostdlib -nostartfiles -Wl,--gc-sections"
    CACHE STRING ""
)

# Утилиты (опционально, для post-build)
find_program(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
find_program(CMAKE_SIZE_UTIL ${TOOLCHAIN_PREFIX}-size)