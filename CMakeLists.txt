# Buzzerino example programs
# Open the Buzzerino folder in VS Code and use "Compile Project" of the
# Raspberry Pi Pico extension to build all example programs at once.
# The .uf2 files end up in build/example_programs/<name>/

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(buzzerino C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Example programs
add_subdirectory(example_programs/breakout)
add_subdirectory(example_programs/rpi_program_selector)
add_subdirectory(example_programs/self_test)
add_subdirectory(example_programs/sidescroller)
add_subdirectory(example_programs/snake)
add_subdirectory(example_programs/tetris)
add_subdirectory(example_programs/weatherstation)
