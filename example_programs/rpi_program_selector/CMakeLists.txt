# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

project(rpi_program_selector C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Executable and source files
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

# Enable/disable stdio
pico_enable_stdio_uart(rpi_program_selector 0)
pico_enable_stdio_usb(rpi_program_selector 1)

# Include directories
target_include_directories(rpi_program_selector PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link libraries
target_link_libraries(rpi_program_selector
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_adc
)

# Create additional output files like .uf2, .bin, .hex
pico_add_extra_outputs(rpi_program_selector)