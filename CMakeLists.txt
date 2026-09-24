# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# MIT License
#
# Copyright (c) 2026 Kevin Thomas
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Author:  Kevin Thomas
# Email:   kevin@mytechnotalent.com
# GitHub:  https://github.com/mytechnotalent/cold-chain-monitor-c-rp2350
# File:    CMakeLists.txt
# Desc:    Configures the RP2350 Pico SDK project and Cold Chain monitor module targets.
# Created: 2026

cmake_minimum_required(VERSION 3.13)

include(FetchContent)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
	set(USERHOME $ENV{USERPROFILE})
else()
	set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
	include(${picoVscode})
endif()

set(PICO_BOARD pico2 CACHE STRING "Board type")

include(pico_sdk_import.cmake)
project(cold_chain_monitor_c_rp2350 C CXX ASM)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(PACKET_ARTIFACT_JSON ${CMAKE_CURRENT_LIST_DIR}/scripts/packet_artifact.json)
set(PACKET_ARTIFACT_HEADER_COMMITTED ${CMAKE_CURRENT_LIST_DIR}/include/packet_artifact.h)
set(PACKET_ARTIFACT_HEADER_GENERATED ${CMAKE_CURRENT_BINARY_DIR}/generated/packet_artifact.h)

add_custom_command(
	OUTPUT ${PACKET_ARTIFACT_HEADER_GENERATED}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/scripts/gen_packet.py
			--from-json ${PACKET_ARTIFACT_JSON}
			--header-out ${PACKET_ARTIFACT_HEADER_GENERATED}
			--check-header-path ${PACKET_ARTIFACT_HEADER_COMMITTED}
	DEPENDS
		${CMAKE_CURRENT_LIST_DIR}/scripts/gen_packet.py
		${PACKET_ARTIFACT_JSON}
		${PACKET_ARTIFACT_HEADER_COMMITTED}
	COMMENT "Regenerating packet artifact header from JSON and checking committed header freshness"
	VERBATIM
)

add_custom_target(check_packet_artifact_header DEPENDS ${PACKET_ARTIFACT_HEADER_GENERATED})

pico_sdk_init()

add_library(crc STATIC src/crc.c)
target_include_directories(crc PUBLIC include)

add_library(sensor STATIC src/sensor.c)
add_dependencies(sensor check_packet_artifact_header)
target_include_directories(sensor PUBLIC include)
target_link_libraries(sensor PUBLIC pico_stdlib crc hardware_i2c)

add_library(display STATIC src/display.c)
target_include_directories(display PUBLIC include)
target_link_libraries(display PUBLIC pico_stdlib hardware_i2c)

add_library(radio STATIC src/radio.c)
add_dependencies(radio check_packet_artifact_header)
target_include_directories(radio PUBLIC include)
target_link_libraries(radio PUBLIC pico_stdlib hardware_i2c)

add_library(status_led STATIC src/status_led.c)
target_include_directories(status_led PUBLIC include)
target_link_libraries(status_led PUBLIC pico_stdlib hardware_i2c hardware_uart)

add_library(button STATIC src/button.c)
target_include_directories(button PUBLIC include)
target_link_libraries(button PUBLIC pico_stdlib hardware_i2c hardware_uart)

add_library(servo STATIC src/servo.c)
target_include_directories(servo PUBLIC include)
target_link_libraries(servo PUBLIC pico_stdlib hardware_pwm hardware_i2c hardware_uart)

add_library(ir_remote STATIC src/ir_remote.c)
target_include_directories(ir_remote PUBLIC include)
target_link_libraries(ir_remote PUBLIC pico_stdlib hardware_i2c hardware_uart)

add_library(crypto STATIC
	src/chacha20.c
	src/poly1305.c
	src/crypto_aead.c
	src/blake2b.c
	src/argon2.c
	src/crypto_kdf.c
	src/envelope.c
)
target_include_directories(crypto PUBLIC include)
target_link_libraries(crypto PUBLIC pico_stdlib pico_rand)

add_library(monitor STATIC src/monitor.c)
add_dependencies(monitor check_packet_artifact_header)
target_include_directories(monitor PUBLIC include)
target_link_libraries(monitor PUBLIC pico_stdlib sensor display radio status_led button servo ir_remote crypto hardware_i2c hardware_pwm)

add_executable(cold_chain_monitor_c_rp2350 src/main.c)
target_link_libraries(cold_chain_monitor_c_rp2350 PRIVATE monitor pico_stdlib)
pico_enable_stdio_uart(cold_chain_monitor_c_rp2350 1)
pico_enable_stdio_usb(cold_chain_monitor_c_rp2350 1)
pico_add_extra_outputs(cold_chain_monitor_c_rp2350)

add_custom_command(
	TARGET cold_chain_monitor_c_rp2350 POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_c_rp2350.elf
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_app.elf
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_c_rp2350.uf2
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_app.uf2
	BYPRODUCTS
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_app.elf
			${CMAKE_CURRENT_BINARY_DIR}/cold_chain_monitor_app.uf2
	COMMENT "Generating backward-compatible cold_chain_monitor_app binaries"
	VERBATIM
)