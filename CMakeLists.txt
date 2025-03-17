cmake_minimum_required(VERSION 3.7)
project(edge)

enable_testing()

# cmake -DCMAKE_BUILD_TYPE=Debug / Release ..
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
# TODO - re enable. This outputs a billion warnings and -Wextra results in a CL.exe exit with code 2. 
# set(CMAKE_CXX_FLAGS "-Wall")
else()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
if (APPLE)
set(CMAKE_CXX_FLAGS "-std=c++17")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (WIN32)
# TODO: Test this.
endif()

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

add_library (edge_lib
    src/address_router.cc
    src/bit_command.cc
    src/call_command.cc
    src/cartridge.cc
    src/cb_command.cc
    src/command_factory.cc
    src/cpu.cc
    src/input_controller.cc
    src/interrupt_controller.cc
    src/jump_command.cc
    src/load_command.cc
    src/math_command.cc
    src/misc_command.cc
    src/mmu.cc
    src/noise_voice.cc
    src/nop_command.cc
    src/pixel_fifo.cc
    src/pulse_voice.cc
    src/ppu.cc
    src/return_command.cc
    src/screen.cc
    src/serial_controller.cc
    src/sound_controller.cc
    src/stack_command.cc
    src/sprite.cc
    src/timer_controller.cc
    src/unimplemented_command.cc
    src/utils.cc
    src/wave_voice.cc)
target_include_directories(edge_lib PUBLIC ${SDL3_INCLUDE_DIR})
target_include_directories(edge_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(edge_lib ${SDL3_LIBRARIES})


add_executable(edge WIN32
    src/system.cc
    src/main.cc)
target_link_libraries(edge edge_lib)
target_link_libraries(edge ${SDL3_LIBRARIES})


target_include_directories(edge PUBLIC ${PROJECT_SOURCE_DIR}/include)
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(edge PRIVATE -Werror -Wall -Wextra)
endif(CMAKE_COMPILER_IS_GNUCXX)

install(TARGETS edge DESTINATION bin)

# Below is from googletest README.md.
# Download and unpack googletest at configure time.
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Tests
add_executable(tests
    tests/address_router_test.cc
    tests/bit_commands_test.cc
    tests/call_command_test.cc
    tests/cartridge_test.cc
    tests/cb_command_test.cc
    tests/cpu_registers_test.cc
    tests/input_controller_test.cc
    tests/interrupt_controller_test.cc
    tests/jump_command_test.cc
    tests/load_command_test.cc
    tests/math_command_test.cc
    tests/misc_command_test.cc
    tests/mmu_test.cc
    tests/noise_voice_test.cc
    tests/ppu_test.cc
    tests/pulse_voice_test.cc
    tests/sound_controller_test.cc
    tests/sprite_test.cc
    tests/stack_test.cc
    tests/timer_controller_test.cc)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tests gtest_main)
target_link_libraries(tests gmock)
target_link_libraries(tests edge_lib)
add_test(NAME basic_tests COMMAND tests)

# Build test ROMS.
# Find RGBDS tools. 
find_program(RGBASM rgbasm)
find_program(RGBLINK rgblink)
find_program(RGBFIX rgbfix)

# Check if tools are found
if(NOT RGBASM OR NOT RGBLINK OR NOT RGBFIX)
    message(FATAL_ERROR "RGBDS tools (rgbasm, rgblink, rgbfix) are required. Install them from https://github.com/gbdev/rgbds .")
endif()

# Source directory for .asm files
set(ASM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/roms)

# List of ROMs to build (without path, just base names)
set(ROMS
    pulse1
    pulse2
    wave
    noise
)

# Custom command to assemble .asm to .o in build directory
foreach(ROM ${ROMS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.o
        COMMAND ${RGBASM} -o ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.o -I ${ASM_SOURCE_DIR} ${ASM_SOURCE_DIR}/${ROM}.asm
        DEPENDS ${ASM_SOURCE_DIR}/${ROM}.asm ${ASM_SOURCE_DIR}/hardware.inc
        COMMENT "Assembling ${ROM}.asm to ${ROM}.o"
    )
endforeach()

# Custom command to link and fix .o to .gb in build directory
foreach(ROM ${ROMS})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.gb
        COMMAND ${RGBLINK} -o ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.gb ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.o && ${RGBFIX} -m MBC3+RAM+BATTERY -r 3 -v ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.gb
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.o
        COMMENT "Linking and fixing ${ROM}.o to ${ROM}.gb"
    )
endforeach()

# Define targets for each ROM
foreach(ROM ${ROMS})
    add_custom_target(${ROM} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${ROM}.gb
    )
endforeach()

# Default target to build all ROMs
add_custom_target(all_roms ALL
    DEPENDS ${ROMS}
)