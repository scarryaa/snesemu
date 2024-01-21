#include "../include/generic_memory_command.hpp"

uint8_t GenericMemoryCommand::read(uint32_t address)
{
	uint32_t actualAddress = (bank * 0x10000) + address;
	return (*memory)[actualAddress];
}

void GenericMemoryCommand::write(uint32_t address, uint8_t value)
{
	uint32_t actualAddress = (bank * 0x10000) + address;
	(*memory)[actualAddress] = value;
}

uint32_t GenericMemoryCommand::getEnd()
{
	return end;
}