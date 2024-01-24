#include "../../include/debug/disassembler.hpp"

Disassembler* Disassembler::instance = nullptr;

Disassembler* Disassembler::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Disassembler();
	}
	return instance;
}

Disassembler::~Disassembler()
{
	delete instance;
}

void Disassembler::disassemble(uint32_t address) {
	uint16_t opcode = memory->read(address);
	Instruction result;

	result.address = address;
	result.opcode = opcode;
	result.operand1 = memory->read(address + 1);
	result.operand2 = memory->read(address + 2);
	result.mnemonic = instruction.mnemonic;
	result.addressingMode = instruction.addressingMode;
	result.length = instruction.bytes;
}