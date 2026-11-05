# rpi_program_selector example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(rpi_program_selector
    main.c
    lcd1602.c
    buttons.c
    menu.c
    aht10.c
    app_weather.c
    app_breakout.c
    app_sidescroller.c
    app_snake.c
    app_tetris.c
    app_selftest.c
    weather_station.c
    rgb_led.c
)

pico_set_program_name(rpi_program_selector "rpi_program_selector")
pico_set_program_version(rpi_program_selector "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rpi_program_selector 0)
pico_enable_stdio_usb(rpi_program_selector 1)

target_link_libraries(rpi_program_selector
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_adc
)

target_include_directories(rpi_program_selector PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(rpi_program_selector)
