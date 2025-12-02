set(SMCP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/gpio_driver.cpp      # ← Исправлен путь
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/timer_driver.cpp     # ← Исправлен путь
    ${CMAKE_SOURCE_DIR}/components/services/led_service.cpp
)

add_executable(${PROJECT_NAME}.elf ${SMCP_SOURCES})

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/startup.s PROPERTIES LANGUAGE ASM)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/components/common
    ${CMAKE_SOURCE_DIR}/components/drivers
    ${CMAKE_SOURCE_DIR}/components/services
    ${CMAKE_CURRENT_SOURCE_DIR}  # для registers.h, и пр.
)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# TODO: make custom commands
# add_custom_target(firmware.bin ALL
#     DEPENDS smcp_firmware.elf
#     COMMAND ${CMAKE_OBJCOPY} -O binary smcp_firmware.elf smcp_firmware.bin
# )