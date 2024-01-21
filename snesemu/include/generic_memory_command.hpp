#ifndef GENERIC_MEMORY_COMMAND_HPP
#define GENERIC_MEMORY_COMMAND_HPP

#include "memory_command.hpp"
#include <vector>

class GenericMemoryCommand : public MemoryCommand
{
public:
    GenericMemoryCommand(std::vector<uint8_t>* memory, uint32_t end, uint8_t bankStart, uint8_t bankEnd) : memory(memory), end(end), bankPair(std::make_pair(bankStart, bankEnd)) {}

    uint8_t read(uint32_t address) override;
    void write(uint32_t address, uint8_t value) override;
    uint32_t getEnd() override;

private:
    std::vector<uint8_t>* memory;
    uint8_t end;
    std::pair<uint8_t, uint8_t> bankPair;
};

#endif