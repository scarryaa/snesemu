#ifndef CARTRIDGE_HPP
#define CARTRIDGE_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include "rom_type.hpp"

class Cartridge {
public:
	Cartridge() : loRom(ROM_SIZE) {}
	void loadRom(std::string path);
	uint8_t read(uint32_t address);
	uint32_t getResetVector();
private:
	const int LOROM_HEADER = 0x7FC0;
	constexpr static size_t ROM_SIZE = 0xFFFFFF;   // 4MB ROM

	uint16_t resetVector;
	std::vector<uint8_t> loRom;
	ROM_TYPE romType = ROM_TYPE::Unknown;
};

#endif