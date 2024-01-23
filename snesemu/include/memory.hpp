#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <string>

class Memory
{
public:
    Memory() :
        wram(WRAM_SIZE),
        sram(SRAM_SIZE),
        loRom(ROM_SIZE),
        ppuRegisters(0x100),
        ppuAPUAndHardware(0x100),
        dmaAndPPU2(0x300),
        dspSuperFXAndHardware(0x1000),
        oldJoypad(0x100),
        dmaPPU2AndHardware(0x300) {}

    uint8_t read(uint32_t address);
    void write(uint32_t address, uint8_t value);

    void loadRom(std::string path);

private:
    std::vector<uint8_t> wram;
    std::vector<uint8_t> sram;
    std::vector<uint8_t> loRom;

    std::vector<uint8_t> ppuRegisters;
    std::vector<uint8_t> ppuAPUAndHardware;
    std::vector<uint8_t> dmaAndPPU2;
    std::vector<uint8_t> dspSuperFXAndHardware;
    std::vector<uint8_t> oldJoypad;
    std::vector<uint8_t> dmaPPU2AndHardware;

    constexpr static size_t WRAM_SIZE = 0x20000;   // 128KB WRAM
    constexpr static size_t SRAM_SIZE = 0x8000;    // 32KB SRAM
    constexpr static size_t ROM_SIZE = 0x400000;   // 4MB ROM
};

#endif