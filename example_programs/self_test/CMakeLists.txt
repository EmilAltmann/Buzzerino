# self_test example program (built from the CMakeLists.txt in the Buzzerino root folder)

add_executable(self_test
    self_test.c
    lcd1602.c
    aht10.c
    weather_station.c
    rgb_led.c
)

pico_set_program_name(self_test "self_test")
pico_set_program_version(self_test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(self_test 0)
pico_enable_stdio_usb(self_test 1)

target_link_libraries(self_test
    pico_stdlib
    pico_cyw43_arch_none
    hardware_i2c
    hardware_adc
    hardware_pwm
)

target_include_directories(self_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(self_test)
