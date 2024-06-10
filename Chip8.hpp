#pragma once
#ifndef CHIP_8_HEADER
#define CHIP_8_HEADER

#include <array>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <vector>
#include <random>

class Chip8 {
public:

    Chip8();

    void loadROM(std::string_view ROMPath);

    void cycle();

    void pressKey(uint8_t key);
    void releaseKey(uint8_t key);

    [[nodiscard]]
    std::array<bool, 64 * 32> getVideo() const noexcept;

private:

    typedef void (Chip8::*Chip8Func)();

    std::array<uint8_t, 16> registers_m;
    std::array<uint8_t, 4096> memory_m;
    std::array<uint16_t, 16> stack_m;
    std::array<bool, 64 * 32> screen_m;
    std::array<bool, 16> keypad_m;
    uint8_t stackPointer_m;
    uint16_t index_m;
    uint16_t pc_m;
    uint8_t delayTimer_m;
    uint8_t soundTimer_m;
    uint16_t opCode_m;

    std::array<Chip8Func, 0xF + 1> functionTable_m;
    std::array<Chip8Func, 0xE + 1> funcion8Table_m;
    std::array<Chip8Func, 0xE + 1> funcion0Table_m;
    std::array<Chip8Func, 0xE + 1> funcionETable_m;
    std::array<Chip8Func, 0x65 + 1> functionFTable_m;

    std::random_device randomDevice_m;
    std::uniform_int_distribution<uint8_t> distribution_m{ 0, 255 };

    void initializeFonts();
    void initializeFunctionTable();
    void decode8();
    void decode0();
    void decodeE();
    void decodeF();

    uint16_t getNNN() const noexcept;
    uint8_t getNN() const noexcept;
    uint8_t getX() const noexcept;
    std::pair<uint8_t, uint8_t> getXY() const noexcept;

    void OP_00e0();
    void OP_00ee();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xnn();
    void OP_4xnn();
    void OP_5xy0();
    void OP_6xnn();
    void OP_7xnn();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xye();
    void OP_9xy0();
    void OP_annn();
    void OP_bnnn();
    void OP_cxnn();
    void OP_dxyn();
    void OP_ex9e();
    void OP_exa1();
    void OP_fx07();
    void OP_fx0a();
    void OP_fx15();
    void OP_fx18();
    void OP_fx29();
    void OP_fx33();
    void OP_fx1E();
    void OP_fx55();
    void OP_fx65();
};

#endif