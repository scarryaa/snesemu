#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <stdint.h>
#include "../memory.hpp"
#include "../cpu.hpp"
#include "instruction.hpp"

class Disassembler {
public:
	static Disassembler* getInstance();
	Instruction disassemble(uint32_t address);

	Disassembler(Disassembler const&) = delete;
	void operator=(Disassembler const&) = delete;

	~Disassembler();
private:
	static Disassembler* instance;
	Memory* memory;
	Cpu* cpu;

	Disassembler() : cpu(cpu), memory(memory) {}
};

#endif