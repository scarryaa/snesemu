#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include "cartridge.hpp"

// TODO refactor this?
class Cpu;
class Ppu;

class Memory
{
public:
    Memory(Cartridge *cartridge) :
        cartridge(cartridge),
        memory(0xFFFFFF),
        ppuRegisters(0x100),
        ppuAPUAndHardware(0x100),
        dmaAndPPU2(0x300),
        dspSuperFXAndHardware(0x1000),
        oldJoypad(0x100),
        dmaPPU2AndHardware(0x300) {}

    void setCpu(Cpu& cpu) {
        this->cpu = &cpu;
    }

    void setPpu(Ppu& ppu) {
        this->ppu = &ppu;
    }

    void startDMA();

    uint8_t read(uint32_t address);
    void write(uint32_t address, uint8_t value);

private:
    Cartridge *cartridge;
    Cpu* cpu;
    Ppu* ppu;

    std::vector<uint8_t> memory;

    std::vector<uint8_t> ppuRegisters;
    std::vector<uint8_t> ppuAPUAndHardware;
    std::vector<uint8_t> dmaAndPPU2;
    std::vector<uint8_t> dspSuperFXAndHardware;
    std::vector<uint8_t> oldJoypad;
    std::vector<uint8_t> dmaPPU2AndHardware;
};

#endif
