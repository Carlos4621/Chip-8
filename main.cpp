#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Chip8Emulator.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "Usage: " << argv[0] << "<Path to ROM>" << endl;
        return -1;
    }

    try {
    Chip8Emulator emu{ 100 };

    emu.init(argv[1]);

    }
    catch(const std::exception& e) {
        cout << e.what() << endl;
    }
}