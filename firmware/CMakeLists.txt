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
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD adafruit_kb2040 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(FeatureDeck C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(FeatureDeck main.cpp   usb_descriptors.c)

pico_set_program_name(FeatureDeck "FeatureDeck")
pico_set_program_version(FeatureDeck "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(FeatureDeck 1)
pico_enable_stdio_usb(FeatureDeck 0)

# Add the standard library to the build
target_link_libraries(FeatureDeck
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(FeatureDeck PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(FeatureDeck 
        hardware_pio
        hardware_timer
        hardware_clocks
        tinyusb_device 
        tinyusb_board
        )

pico_add_extra_outputs(FeatureDeck)

