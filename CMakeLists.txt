cmake_minimum_required(VERSION 3.10)
project(edge)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (WIN32)
set(SDL2_DIR "C://Users/edwar/Documents/Dev/SDL2-2.0.10")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library (edge_lib
    src/AddressRouter.cpp
    src/BitCommand.cpp
    src/CallCommand.cpp
    src/CBCommand.cpp
    src/Command.cpp
    src/CommandFactory.cpp
    src/CPU.cpp
    src/JumpCommand.cpp
    src/LoadCommand.cpp
    src/MathCommand.cpp
    src/MiscCommand.cpp
    src/MMU.cpp
    src/NopCommand.cpp
    src/PixelFIFO.cpp
    src/PPU.cpp
    src/ReturnCommand.cpp
    src/ROM.cpp
    src/Screen.cpp
    src/serial_controller.cpp
    src/StackCommand.cpp
    src/Sprite.cpp
    src/UnimplementedCommand.cpp
    src/Utils.cpp)
target_include_directories(edge_lib PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(edge_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(edge_lib ${SDL2_LIBRARIES})


add_executable(edge WIN32
    src/main.cpp)
target_link_libraries(edge edge_lib)
target_link_libraries(edge ${SDL2_LIBRARIES})


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
    tests/address_router_test.cpp
    tests/bit_commands_test.cpp
    tests/call_command_test.cpp
    tests/cb_command_test.cpp
    tests/cpu_registers_test.cpp
    tests/jump_command_test.cpp
    tests/math_command_test.cpp
    tests/misc_command_test.cpp
    tests/mmu_test.cpp
    tests/ppu_test.cpp
    tests/sprite_test.cpp
    tests/stack_test.cpp)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tests gtest_main)
target_link_libraries(tests edge_lib)
add_test(NAME basic_tests COMMAND tests)
