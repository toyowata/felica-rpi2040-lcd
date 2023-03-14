# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.19.0 FATAL_ERROR)

set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
set(APP_TARGET felica-rpi2040-lcd)

include(${MBED_PATH}/tools/cmake/app.cmake)

project(${APP_TARGET})

add_subdirectory(${MBED_PATH})

add_executable(${APP_TARGET})

target_include_directories(${APP_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Fonts
        ${CMAKE_CURRENT_SOURCE_DIR}/ST7735S_TFT
)

target_sources(${APP_TARGET}
    PRIVATE
        main.cpp
        RCS620S.cpp
        ST7735S_TFT/GraphicsDisplay.cpp
        ST7735S_TFT/ST7735S.cpp
        ST7735S_TFT/TextDisplay.cpp
        mbed-os/targets/TARGET_RASPBERRYPI/TARGET_RP2040/pico-sdk/host/hardware_timer/timer.c
)

target_link_libraries(${APP_TARGET}
    PRIVATE
        mbed-os
        mbed-rp2040
        mbed-raspberry-pi-pico
)

mbed_set_post_build(${APP_TARGET})

option(VERBOSE_BUILD "Have a verbose build process")
if(VERBOSE_BUILD)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
