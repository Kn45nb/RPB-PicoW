# Generated Cmake Pico project file

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
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(KN45N-BMS-test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(KN45N-BMS-test KN45N-BMS-test.cpp )

pico_set_program_name(KN45N-BMS-test "KN45N-BMS-test")
pico_set_program_version(KN45N-BMS-test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(KN45N-BMS-test 0)
pico_enable_stdio_usb(KN45N-BMS-test 0)

# Add the standard library to the build
target_link_libraries(KN45N-BMS-test
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(KN45N-BMS-test PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(KN45N-BMS-test 
        hardware_spi
        hardware_i2c
        pico_cyw43_arch_none
        )

pico_add_extra_outputs(KN45N-BMS-test)

# I2C
add_executable(slave_mem_i2c
    slave_mem_i2c.c
    )
target_link_libraries(slave_mem_i2c
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    )
pico_add_extra_outputs(slave_mem_i2c)
example_auto_set_url(slave_mem_i2c)

add_executable(slave_mem_i2c_burst
    slave_mem_i2c_burst.c
    )
target_link_libraries(slave_mem_i2c_burst
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    )
pico_add_extra_outputs(slave_mem_i2c_burst)
example_auto_set_url(slave_mem_i2c_burst)