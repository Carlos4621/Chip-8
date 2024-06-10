#pragma once
#ifndef CHIP_8_EMULATOR_HEADER
#define CHIP_8_EMULATOR_HEADER

#include <string_view>
#include "Chip8.hpp"
#include <SFML/Graphics.hpp>

class Chip8Emulator {
public:
    explicit Chip8Emulator(size_t maxFPS = 60);

    void init(std::string_view ROMPath);

private:

    Chip8 chip_m;
    size_t maxFPS_m;

    sf::RenderWindow mainWindow_m;

    sf::RectangleShape pixel{ {10, 10} };

    void refreshWindow();
    void manageEvents();

};

#endif