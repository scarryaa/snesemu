#include "../include/memory.hpp"
#include "../include/logger.hpp"
#include "../include/generic_memory_command.hpp"

Memory::Memory() {
	memory.resize(0x100000);	// 1 MB

    // add entries
    GenericMemoryCommand loRam(&memory, 0x1FFF, 0x00, 0x3F);
    // TODO make PPU not generic
    GenericMemoryCommand PPU1(&memory, 0x21FF, 0x00, 0x3F);
    GenericMemoryCommand superFX(&memory, 0x3FFF, 0x00, 0x3F);

    memoryCommands.emplace(std::make_pair(0x0000, &loRam));
}

uint8_t Memory::read(uint32_t address)
{
    for (const auto& command : memoryCommands)
    {
        if (address >= command.first && address < command.second->getEnd())
        {
            return command.second->read(address);
        }
    }
    Logger::getInstance()->logError("Invalid address read: " + std::to_string(address));
}

void Memory::write(uint32_t address, uint8_t value)
{
    for (const auto& command : memoryCommands)
    {
        if (address >= command.first && address < command.second->getEnd())
        {
            command.second->write(address, value);
            return;
        }
    }
    Logger::getInstance()->logError("Invalid address write: " + std::to_string(address));
}

void Memory::loadRom(std::string path)
{
	
}