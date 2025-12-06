cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/toolchains/gcc-arm-none-eabi.cmake)
    message(STATUS "Using default toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(SMCP C ASM)

set(SOURCES
    startup.s
    main.c
    drivers/smcp_gpio.c
    drivers/smcp_gp_timer.c
    services/led_service.c
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libs/libopencm3/include
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE STM32F1)

set(LIBOPENCM3_LIB ${CMAKE_SOURCE_DIR}/libs/libopencm3/lib/libopencm3_stm32f1.a)
if(NOT EXISTS ${LIBOPENCM3_LIB})
    message(FATAL_ERROR "libopencm3 not built. Run:\n  cd libs/libopencm3 && make TARGETS='stm32/f1'")
endif()
target_link_libraries(${PROJECT_NAME}.elf ${LIBOPENCM3_LIB})

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating BIN/HEX and printing size"
)