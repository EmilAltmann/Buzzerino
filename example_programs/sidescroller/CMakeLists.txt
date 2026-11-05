# sidescroller example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(sidescroller
    sidescroller.c
    lcd1602.c
)

pico_set_program_name(sidescroller "sidescroller")
pico_set_program_version(sidescroller "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(sidescroller 0)
pico_enable_stdio_usb(sidescroller 1)

target_link_libraries(sidescroller
    pico_stdlib
    hardware_i2c
    hardware_adc
    hardware_pwm
)

target_include_directories(sidescroller PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(sidescroller)

# Copy the finished .uf2 next to the sources, so it can be downloaded from GitHub
add_custom_command(TARGET sidescroller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:sidescroller>/sidescroller.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/sidescroller.uf2
)
