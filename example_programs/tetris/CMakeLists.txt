# tetris example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(tetris
    tetris.c
    lcd1602.c
)

pico_set_program_name(tetris "tetris")
pico_set_program_version(tetris "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(tetris 0)
pico_enable_stdio_usb(tetris 0)

target_link_libraries(tetris
    pico_stdlib
    hardware_adc
)

target_include_directories(tetris PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(tetris)

# Copy the finished .uf2 next to the sources, so it can be downloaded from GitHub
add_custom_command(TARGET tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:tetris>/tetris.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/tetris.uf2
)
