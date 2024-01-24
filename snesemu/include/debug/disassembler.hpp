#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <stdint.h>
#include "../memory.hpp"
#include "../cpu.hpp"
#include "../../include/debug/instruction.hpp"

class Disassembler {
public:
	Disassembler(Cpu* cpu, Memory* memory) : cpu(cpu), memory(memory) {}
	~Disassembler();
	Instruction disassemble(uint32_t address);

private:
	static Disassembler* instance;
	Memory* memory;
	Cpu* cpu;
};

#endif