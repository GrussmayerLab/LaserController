# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

message($ENV{PICO_SDK_PATH})

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PROJECT_NAME "LaserController")
# Set name of project (as PROJECT_NAME) and C/C   standards
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.cpp
)

# Show CMake the lib folder with its corresponding external libraries
add_subdirectory(lib/mcp4922)


target_include_directories(${PROJECT_NAME}
    PUBLIC lib/mcp4922
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})


# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
    cmsis_core
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_i2c
    hardware_irq
    hardware_adc
    hardware_pwm
    mcp4922
)

# Enable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)