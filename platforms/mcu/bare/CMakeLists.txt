add_executable(${PROJECT_NAME}.elf 
    ${CMAKE_CURRENT_SOURCE_DIR}/startup.s
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/startup.s PROPERTIES LANGUAGE ASM)

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

if(USE_MODERN_CPP)
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE NUKE_SOKOL_ASS_WITH_MODERN_CPP)
    message(STATUS "Modern C++ syntax enabled")
else()
    message(STATUS "Modern C++ syntax disabled. Old C style activated")
endif()

# TODO: make custom commands
# add_custom_target(firmware.bin ALL
#     DEPENDS smcp_firmware.elf
#     COMMAND ${CMAKE_OBJCOPY} -O binary smcp_firmware.elf smcp_firmware.bin
# )