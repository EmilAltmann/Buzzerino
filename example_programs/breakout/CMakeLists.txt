# breakout example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(breakout
    breakout.c
    lcd1602.c
)

pico_set_program_name(breakout "breakout")
pico_set_program_version(breakout "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(breakout 0)
pico_enable_stdio_usb(breakout 0)

target_link_libraries(breakout
    pico_stdlib
    hardware_adc
)

target_include_directories(breakout PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(breakout)

# Copy the finished .uf2 next to the sources, so it can be downloaded from GitHub
add_custom_command(TARGET breakout POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:breakout>/breakout.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/breakout.uf2
)
