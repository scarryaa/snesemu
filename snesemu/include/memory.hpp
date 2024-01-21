#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include "memory_command.hpp"
#include <map>
#include <array>

constexpr size_t WRAM_SIZE = 0x20000;	// 128KB WRAM
constexpr size_t SRAM_SIZE = 0x8000;	// 32KB SRAM
constexpr size_t ROM_SIZE = 0x400000;	// 4MB ROM

class Memory
{
public:
	Memory();

	uint8_t read(uint32_t address);
	void write(uint32_t address, uint8_t value);

	void loadRom(std::string path);
private:
	std::array<uint8_t, WRAM_SIZE> wram;
	std::array<uint8_t, SRAM_SIZE> sram;
	std::array<uint8_t, ROM_SIZE> loRom;

	std::array<uint8_t, 0x100> ppuRegisters;
	std::array<uint8_t, 0x100> apuAndHardware;
	std::array<uint8_t, 0x300> dmaAndPPU2;
};

#endif