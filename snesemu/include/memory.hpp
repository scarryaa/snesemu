#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include "memory_command.hpp"
#include <map>

class Memory
{
public:
	Memory();

	uint8_t read(uint32_t address);
	void write(uint32_t address, uint8_t value);

	void loadRom(std::string path);
private:
	std::map<uint32_t, MemoryCommand*> memoryCommands;
	std::vector<uint8_t> memory;
};

#endif