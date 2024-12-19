# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
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
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(Milestone_2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Milestone_2 
    Milestone_2.c 
    LCD.c
    servo_pwm.c
    keyboard.c
    ultrasonic.c
    rfid.c
    mfrc522.c
    alarm.c
)

pico_set_program_name(Milestone_2 "Milestone_2")
pico_set_program_version(Milestone_2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Milestone_2 0)
pico_enable_stdio_usb(Milestone_2 1)

# Add the standard library to the build
target_link_libraries(Milestone_2
        pico_stdlib
        )

# Add FreeRTOS Kernel source files
file(GLOB FreeRTOS_src ${FREERTOS_KERNEL_PATH}/*.c)

# Create a FreeRTOS static library
add_library(FreeRTOS STATIC
    ${FreeRTOS_src}
    ${FREERTOS_KERNEL_PATH}/event_groups.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/stream_buffer.c
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_3.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(FreeRTOS PUBLIC
    ${FREERTOS_KERNEL_PATH}/
	${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0
)


# Add the standard include files to the build
target_include_directories(Milestone_2 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)


# Add any user requested libraries
target_link_libraries(Milestone_2 
        hardware_pwm
        hardware_i2c
        hardware_spi
        pico_multicore
        FreeRTOS
        )

pico_add_extra_outputs(Milestone_2)