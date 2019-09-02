cmake_minimum_required(VERSION 3.10)
project(edge)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

add_executable(edge
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
    src/PPU.cpp
    src/ReturnCommand.cpp
    src/ROM.cpp
    src/StackCommand.cpp
    src/Sprite.cpp
    src/UnimplementedCommand.cpp
    src/Utils.cpp
    src/main.cpp)

target_include_directories(edge PUBLIC ${PROJECT_SOURCE_DIR}/include)
install(TARGETS edge DESTINATION bin)

# CXX_FLAGS := -Wall -Wextra -std=c++17 -g