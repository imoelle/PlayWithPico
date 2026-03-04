cmake_minimum_required(VERSION 3.13)

# ==============================================================================
# PROJECT CONFIGURATION HEADER
# ------------------------------------------------------------------------------
# Project:      PicoTemplateProject
# Description:  Professional boilerplate for Pico 2 W (RP2350).
#               Optimized for VS Code and SDK 2.2.0.
#
# Version History:
# v1.2.0 | 2026-02-25 | Final Version with USB-Stdio & SDK 2.2.0 Integration
# ==============================================================================

# 1. ENVIRONMENT SETUP
# ------------------------------------------------------------------------------
# Set project name and standards
set(PROJECT_NAME PicoTemplateProject)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Target Board: Raspberry Pi Pico 2 W (RP2350)
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# --- VS CODE EXTENSION BOOTSTRAPPING ---
# Logic required for the Raspberry Pi Pico VS Code Extension to function
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

# Import the official Raspberry Pi Pico SDK bootstrapping script
include(pico_sdk_import.cmake)

# 2. INITIALIZATION
# ------------------------------------------------------------------------------
project(${PROJECT_NAME} C CXX ASM)
pico_sdk_init()

# 3. EXECUTABLE & SOURCE FILES
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME} main.cpp)

# Metadata for the compiled binary
pico_set_program_name(${PROJECT_NAME} "${PROJECT_NAME}")
pico_set_program_version(${PROJECT_NAME} "0.1")

# 4. LINK LIBRARIES & INCLUDES
# ------------------------------------------------------------------------------
# Add the standard library and project directories
target_link_libraries(${PROJECT_NAME} pico_stdlib)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# --- USER LIBRARIES ---
# Add additional libraries here (e.g., pico_cyw43_arch_none)
target_link_libraries(${PROJECT_NAME} 
        
)

# 5. IO CONFIGURATION
# ------------------------------------------------------------------------------
# 1 = ON (USB Serial), 0 = OFF (UART Pins)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# 6. POST-BUILD STEPS
# ------------------------------------------------------------------------------
# Generate .uf2 for flashing, plus .elf, .bin and .map files
pico_add_extra_outputs(${PROJECT_NAME})