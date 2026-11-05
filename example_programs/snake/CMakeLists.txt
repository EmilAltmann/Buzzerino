# snake example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(snake
    snake.c
    lcd1602.c
)

pico_set_program_name(snake "snake")
pico_set_program_version(snake "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(snake 0)
pico_enable_stdio_usb(snake 0)

target_link_libraries(snake
    pico_stdlib
)

target_include_directories(snake PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(snake)

# Copy the finished .uf2 next to the sources, so it can be downloaded from GitHub
add_custom_command(TARGET snake POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:snake>/snake.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/snake.uf2
)
