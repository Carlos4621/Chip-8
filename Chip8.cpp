#include "Chip8.hpp"

static constexpr uint16_t START_ADDRESS{ 0x200 };
static constexpr uint16_t FONT_START_ADRESS{ 0x50 };
static constexpr uint8_t LAST_REGISTER_F{ 15 };

Chip8::Chip8() 
: pc_m{ START_ADDRESS }
{
    initializeFunctionTable();
    initializeFonts();
}

void Chip8::loadROM(std::string_view ROMPath)
{
    std::ifstream ROMData{ ROMPath.data(), std::ios::binary | std::ios::ate };

    if (!ROMData.is_open()) {
        throw std::runtime_error{ "Unable to open the ROM" };
    }

    const auto ROMSize{ ROMData.tellg() };

    std::vector<char> buffer(ROMSize);

    ROMData.seekg(0, std::ios::beg);
    ROMData.read(buffer.data(), buffer.size());
    ROMData.close();

    for (size_t i { 0 }; i < buffer.size(); ++i) {
        memory_m[START_ADDRESS + i] = buffer[i];
    }
}

void Chip8::cycle() {
	opCode_m = (memory_m[pc_m] << 8u) | memory_m[pc_m + 1];

	pc_m += 2;

	(this->*(functionTable_m[(opCode_m & 0xF000) >> 12]))();

	if (delayTimer_m > 0)
	{
		--delayTimer_m;
	}

	if (soundTimer_m > 0)
	{
		--soundTimer_m;
	}
}

void Chip8::pressKey(uint8_t key) {
    keypad_m[key] = true;
}

void Chip8::releaseKey(uint8_t key) {
    keypad_m[key] = false;
}

std::array<bool, 64 * 32> Chip8::getVideo() const noexcept {
    return screen_m;
}

void Chip8::OP_00e0()
{
    screen_m.fill(false);
}

void Chip8::OP_00ee() {
    pc_m = stack_m[--stackPointer_m];
}

void Chip8::OP_1nnn() {
    pc_m = getNNN();
}

void Chip8::OP_2nnn() {
    stack_m[stackPointer_m++] = pc_m;
    pc_m = getNNN();
}

void Chip8::OP_3xnn() {
    const auto nn{ getNN() };
    const auto x{ getX() };

    if (nn == registers_m[x]) {
        pc_m += 2;
    }
}

void Chip8::OP_4xnn() {
    const auto nn{ getNN() };
    const auto x{ getX() };

    if (nn != registers_m[x]) {
        pc_m += 2;
    }
}

void Chip8::OP_5xy0() {
    const auto [x, y]{ getXY() };

    if (registers_m[x] == registers_m[y]) {
        pc_m += 2;
    }
    
}

void Chip8::OP_6xnn() {
    const auto nn{ getNN() };
    const auto x{ getX() };

    registers_m[x] = nn;
}

void Chip8::OP_7xnn() {
    const auto nn{ getNN() };
    const auto x{ getX() };

    registers_m[x] += nn;
}

void Chip8::OP_8xy0() {
    const auto [x, y]{ getXY() };

    registers_m[x] = registers_m[y];
}

void Chip8::OP_8xy1() {
    const auto [x, y]{ getXY() };

    registers_m[x] |= registers_m[y];
}

void Chip8::OP_8xy2() {
    const auto [x, y]{ getXY() };

    registers_m[x] &= registers_m[y];
}

void Chip8::OP_8xy3() {
    const auto [x, y]{ getXY() };

    registers_m[x] ^= registers_m[y];
}

void Chip8::OP_8xy4() {
    const auto [x, y]{ getXY() };

    const auto sum{ registers_m[x] + registers_m[y] };

    registers_m[LAST_REGISTER_F] = (sum > 255 ? 1 : 0);

    registers_m[x] = sum & 0xFF;
}

void Chip8::OP_8xy5() {
    const auto [x, y]{ getXY() };

    registers_m[LAST_REGISTER_F] = (registers_m[x] > registers_m[y] ? 1 : 0);

    registers_m[x] -= registers_m[y];
}

void Chip8::OP_8xy6() {
    const auto [x, y]{ getXY() };

    registers_m[LAST_REGISTER_F] = registers_m[y] & 0x1;

    registers_m[x] = registers_m[y] >> 1;
}

void Chip8::OP_8xy7() {
    const auto [x, y]{ getXY() };

    registers_m[LAST_REGISTER_F] = (registers_m[y] > registers_m[x] ? 1 : 0 );

    registers_m[x] = registers_m[y] - registers_m[x];
}

void Chip8::OP_8xye() {
    const auto [x, y]{ getXY() };

    registers_m[LAST_REGISTER_F] = registers_m[y] & 0x1;

    registers_m[x] = registers_m[y] << 1;
}

void Chip8::OP_9xy0() {
    const auto [x, y]{ getXY() };

    if (registers_m[x] != registers_m[y]) {
        pc_m += 2;
    }
}

void Chip8::OP_annn() {
    const auto nnn{ getNNN() };

    index_m = nnn;
}

void Chip8::OP_bnnn() {
    const auto nnn{ getNNN() };

    index_m = nnn + registers_m[0];
}

void Chip8::OP_cxnn() {
    const auto nn{ getNN() };
    const auto x{ getX() };

    registers_m[x] = distribution_m(randomDevice_m) & nn;
}

void Chip8::OP_dxyn() {
    const auto [x, y]{ getXY() };
    const auto height{ opCode_m & 0x000F };

    registers_m[LAST_REGISTER_F] = 0;

    for (unsigned int row{ 0 }; row < height; ++row) {

        uint8_t spriteByte = memory_m[index_m + row];

        for (unsigned int col = 0; col < 8; ++col) {

            uint8_t spritePixel = spriteByte & (0x80 >> col);

            if (spritePixel != 0) {
                unsigned int screenX = (registers_m[x] + col) % 64;
                unsigned int screenY = (registers_m[y] + row) % 32;
                unsigned int screenIndex = screenY * 64 + screenX;

                if (screen_m[screenIndex] == 1) {
                    registers_m[LAST_REGISTER_F] = 1;
                }

                screen_m[screenIndex] ^= 1;
            }
        }
    }
}

void Chip8::OP_ex9e() {
    const auto x{ getX() };

    if (keypad_m[registers_m[x]]) {
        pc_m += 2;
    }
    
}

void Chip8::OP_exa1() {
    const auto x{ getX() };

    if (!keypad_m[registers_m[x]]) {
        pc_m += 2;
    }
    
}

void Chip8::OP_fx07() {
    const auto x{ getX() };

    registers_m[x] = delayTimer_m;
}

void Chip8::OP_fx0a() {
    const auto x{ getX() };

    bool pressed{ false };

    for (size_t i{ 0 }; i < 16; ++i) {
        if (keypad_m[i]) {
            registers_m[x] = i;
            pressed = true;
            break;
        }
        
    }
    
    if (!pressed) {
        pc_m -= 2;
    }
}

void Chip8::OP_fx15() {
    const auto x{ getX() };

    delayTimer_m = registers_m[x];
}

void Chip8::OP_fx18() {
    const auto x{ getX() };

    soundTimer_m = registers_m[x];
}

void Chip8::OP_fx29() {
    const auto x{ getX() };

    index_m = FONT_START_ADRESS + (5 * registers_m[x]);
}

void Chip8::OP_fx33() {
    const auto x{ getX() };
    auto value{ registers_m[x] };

    memory_m[index_m + 2] = value % 10;
    value /= 10;

    memory_m[index_m + 1] = value % 10;
    value /= 10;

    memory_m[index_m] = value % 10;
}

void Chip8::OP_fx1E() {
    const auto x{ getX() };

    index_m += registers_m[x];
}

void Chip8::OP_fx55() {
    const auto x{ getX() };

    for (size_t i = 0; i <= x; ++i) {
        memory_m[index_m + i] = registers_m[i];
    }

    index_m += x + 1;
}

void Chip8::OP_fx65() {
    const auto x{ getX() };

    for (size_t i = 0; i <= x; ++i) {
        registers_m[i] = memory_m[index_m + i];
    }

    index_m += x + 1;
}

void Chip8::initializeFonts() {

    std::vector<uint8_t> fontset
    {
	    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (size_t i { 0 }; i < fontset.size(); ++i) {
        memory_m[FONT_START_ADRESS + i] = fontset[i];
    }

}

void Chip8::initializeFunctionTable() {
    funcion0Table_m[0x0] = &Chip8::OP_00e0;
    funcion0Table_m[0xE] = &Chip8::OP_00ee;

    functionTable_m[0x0] = &Chip8::decode0;

    functionTable_m[0x1] = &Chip8::OP_1nnn;
    functionTable_m[0x2] = &Chip8::OP_2nnn;
    functionTable_m[0x3] = &Chip8::OP_3xnn;
    functionTable_m[0x4] = &Chip8::OP_4xnn;
    functionTable_m[0x5] = &Chip8::OP_5xy0;
    functionTable_m[0x6] = &Chip8::OP_6xnn;
    functionTable_m[0x7] = &Chip8::OP_7xnn;

    funcion8Table_m[0x0] = &Chip8::OP_8xy0;
    funcion8Table_m[0x1] = &Chip8::OP_8xy1;
    funcion8Table_m[0x2] = &Chip8::OP_8xy2;
    funcion8Table_m[0x3] = &Chip8::OP_8xy3;
    funcion8Table_m[0x4] = &Chip8::OP_8xy4;
    funcion8Table_m[0x5] = &Chip8::OP_8xy5;
    funcion8Table_m[0x6] = &Chip8::OP_8xy6;
    funcion8Table_m[0x7] = &Chip8::OP_8xy7;
    funcion8Table_m[0xe] = &Chip8::OP_8xye;

    functionTable_m[0x8] = &Chip8::decode8;

    functionTable_m[0x9] = &Chip8::OP_9xy0;
    functionTable_m[0xA] = &Chip8::OP_annn;
    functionTable_m[0xB] = &Chip8::OP_bnnn;
    functionTable_m[0xC] = &Chip8::OP_cxnn;
    functionTable_m[0xD] = &Chip8::OP_dxyn;

    funcionETable_m[0x1] = &Chip8::OP_exa1;
    funcionETable_m[0xE] = &Chip8::OP_ex9e;

    functionTable_m[0xE] = &Chip8::decodeE;

    functionFTable_m[0x7] = &Chip8::OP_fx07;
    functionFTable_m[0xA] = &Chip8::OP_fx0a;
    functionFTable_m[0x15] = &Chip8::OP_fx15;
    functionFTable_m[0x18] = &Chip8::OP_fx18;
    functionFTable_m[0x1E] = &Chip8::OP_fx1E;
    functionFTable_m[0x29] = &Chip8::OP_fx29;
    functionFTable_m[0x33] = &Chip8::OP_fx33;
    functionFTable_m[0x55] = &Chip8::OP_fx55;
    functionFTable_m[0x65] = &Chip8::OP_fx65;

    functionTable_m[0xF] = &Chip8::decodeF;
}

void Chip8::decode8() {
    (this->*(funcion8Table_m[opCode_m & 0x000F]))();
}

void Chip8::decode0() {
    (this->*(funcion0Table_m[opCode_m & 0x000F]))();
}

void Chip8::decodeE() {
    (this->*(funcionETable_m[opCode_m & 0x000F]))();
}

void Chip8::decodeF() {
    (this->*functionFTable_m[opCode_m & 0x00FF])();
}

uint16_t Chip8::getNNN() const noexcept {
    return opCode_m & 0x0FFF;
}

uint8_t Chip8::getNN() const noexcept {
    return opCode_m & 0x00FF;
}

uint8_t Chip8::getX() const noexcept {
    return (opCode_m & 0x0F00) >> 8;
}

std::pair<uint8_t, uint8_t> Chip8::getXY() const noexcept {
    return { getX(), ((opCode_m & 0x00F0) >> 4) };
}
