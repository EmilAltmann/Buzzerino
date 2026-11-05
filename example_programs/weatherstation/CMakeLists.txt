# weatherstation example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(weatherstation
    weatherstation.c
    lcd1602.c
    aht10.c
)

pico_set_program_name(weatherstation "weatherstation")
pico_set_program_version(weatherstation "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(weatherstation 0)
pico_enable_stdio_usb(weatherstation 0)

target_link_libraries(weatherstation
    pico_stdlib
    hardware_i2c
    hardware_pwm
)

target_include_directories(weatherstation PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(weatherstation)

# Copy the finished .uf2 next to the sources, so it can be downloaded from GitHub
add_custom_command(TARGET weatherstation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE_DIR:weatherstation>/weatherstation.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/weatherstation.uf2
)
