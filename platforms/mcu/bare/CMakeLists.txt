add_executable(${PROJECT_NAME}.elf 
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/startup.s PROPERTIES LANGUAGE ASM)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# TODO: make custom commands
# add_custom_target(firmware.bin ALL
#     DEPENDS smcp_firmware.elf
#     COMMAND ${CMAKE_OBJCOPY} -O binary smcp_firmware.elf smcp_firmware.bin
# )