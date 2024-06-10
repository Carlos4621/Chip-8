#include "Chip8Emulator.hpp"

Chip8Emulator::Chip8Emulator(size_t maxFPS)
: maxFPS_m{ maxFPS }
, mainWindow_m{ sf::VideoMode{640, 320}, "Chip-8" } 
{
    pixel.setFillColor(sf::Color::White);
}

void Chip8Emulator::init(std::string_view ROMPath) {

    chip_m.loadROM(ROMPath);
    
    while (mainWindow_m.isOpen()) {

        manageEvents();
        refreshWindow();
        mainWindow_m.display();
        sf::sleep(sf::milliseconds(1000 / maxFPS_m));
        mainWindow_m.clear();
        chip_m.cycle();
    }
    
}

void Chip8Emulator::refreshWindow() {
    const auto frame{ chip_m.getVideo() };

    for (size_t i{ 0 }; i < frame.size(); ++i) {
        if (frame[i]) {
            size_t x{ i / 32 * 5 };
            size_t y{ i % 64 * 10};

            pixel.setPosition(y, x);

            mainWindow_m.draw(pixel);
        }
    }
}

void Chip8Emulator::manageEvents() {
    sf::Event e;

    while (mainWindow_m.pollEvent(e)) {
        switch (e.type) {
        case sf::Event::Closed:
            mainWindow_m.close();
            break;
        
        case sf::Event::KeyPressed:
            switch (e.key.code) {
            case sf::Keyboard::Num1:
                chip_m.pressKey(0x1);
                break;

            case sf::Keyboard::Num2:
                chip_m.pressKey(0x2);
                break;

            case sf::Keyboard::Num3:
                chip_m.pressKey(0x3);
                break;

            case sf::Keyboard::Num4:
                chip_m.pressKey(0xC);
                break;

            case sf::Keyboard::Q:
                chip_m.pressKey(0x4);
                break;

            case sf::Keyboard::W:
                chip_m.pressKey(0x5);
                break;

            case sf::Keyboard::E:
                chip_m.pressKey(0x6);
                break;

            case sf::Keyboard::R:
                chip_m.pressKey(0xD);
                break;
            
            case sf::Keyboard::A:
                chip_m.pressKey(0x7);
                break;

            case sf::Keyboard::S:
                chip_m.pressKey(0x8);
                break;

            case sf::Keyboard::D:
                chip_m.pressKey(0x9);
                break;

            case sf::Keyboard::F:   
                chip_m.pressKey(0xE);
                break;

            case sf::Keyboard::Z:
                chip_m.pressKey(0xA);
                break;

            case sf::Keyboard::X:
                chip_m.pressKey(0x0);
                break;

            case sf::Keyboard::C:
                chip_m.pressKey(0xB);
                break;

            case sf::Keyboard::V:
                chip_m.pressKey(0xF);
                break;
            
            default:
                break;
            }

            break;

            case sf::Event::KeyReleased:
            switch (e.key.code) {
            case sf::Keyboard::Num1:
                chip_m.releaseKey(0x1);
                break;

            case sf::Keyboard::Num2:
                chip_m.releaseKey(0x2);
                break;

            case sf::Keyboard::Num3:
                chip_m.releaseKey(0x3);
                break;

            case sf::Keyboard::Num4:
                chip_m.releaseKey(0xC);
                break;

            case sf::Keyboard::Q:
                chip_m.releaseKey(0x4);
                break;

            case sf::Keyboard::W:
                chip_m.releaseKey(0x5);
                break;

            case sf::Keyboard::E:
                chip_m.pressKey(0x6);
                break;

            case sf::Keyboard::R:
                chip_m.releaseKey(0xD);
                break;
            
            case sf::Keyboard::A:
                chip_m.releaseKey(0x7);
                break;

            case sf::Keyboard::S:
                chip_m.releaseKey(0x8);
                break;

            case sf::Keyboard::D:
                chip_m.releaseKey(0x9);
                break;

            case sf::Keyboard::F:   
                chip_m.releaseKey(0xE);
                break;

            case sf::Keyboard::Z:
                chip_m.releaseKey(0xA);
                break;

            case sf::Keyboard::X:
                chip_m.releaseKey(0x0);
                break;

            case sf::Keyboard::C:
                chip_m.releaseKey(0xB);
                break;

            case sf::Keyboard::V:
                chip_m.releaseKey(0xF);
                break;
            
            default:
                break;
            }

        default:
            break;
        }
    }
    
}
