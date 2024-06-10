cmake_minimum_required(VERSION 3.18)
project(chip)

find_package(SFML COMPONENTS graphics window system audio REQUIRED)

add_executable(chip main.cpp Chip8.cpp Chip8Emulator.cpp)
target_link_libraries(chip sfml-graphics sfml-window sfml-system sfml-audio)