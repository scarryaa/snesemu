#ifndef MEMORY_COMMAND_HPP
#define MEMORY_COMMAND_HPP

#include <cstdint>

class MemoryCommand
{
public:
	virtual uint8_t read(uint32_t address) = 0;
	virtual void write(uint32_t address, uint8_t value) = 0;
	virtual uint32_t getEnd() = 0;
};

#endif