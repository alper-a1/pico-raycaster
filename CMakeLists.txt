# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-raycaster C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ---- ST7735 library ----
file(GLOB ST7735_LIB_SOURCES CONFIGURE_DEPENDS "lib/st7735/src/*.cpp")

add_library(ST7735 ${ST7735_LIB_SOURCES})

target_include_directories(ST7735 PUBLIC lib/st7735/include)

target_link_libraries(ST7735 pico_stdlib hardware_spi)
# ------------------------

# ---- Fixed Point library ----

file(GLOB FIXED_POINT_LIB_SOURCES CONFIGURE_DEPENDS "lib/fixed_point/src/*.cpp")

add_library(FIXED_POINT_LIB ${FIXED_POINT_LIB_SOURCES})

target_include_directories(FIXED_POINT_LIB PUBLIC lib/fixed_point/include)

target_link_libraries(FIXED_POINT_LIB pico_stdlib)
# ------------------------

# Add executable. Default name is the project name, version 0.1

add_executable(pico-raycaster pico-raycaster.cpp)

pico_set_program_name(pico-raycaster "pico-raycaster")
pico_set_program_version(pico-raycaster "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-raycaster 0)
pico_enable_stdio_usb(pico-raycaster 1)

# Add the standard library to the build
target_link_libraries(pico-raycaster
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico-raycaster PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any user requested libraries
target_link_libraries(pico-raycaster 
        hardware_spi
        hardware_gpio
        hardware_adc
        ST7735
        FIXED_POINT_LIB
        )

pico_add_extra_outputs(pico-raycaster)

