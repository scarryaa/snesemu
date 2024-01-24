#include "../../include/debug/disassembler.hpp"
#include "../../include/debug/opcode.hpp"

Disassembler* Disassembler::instance = nullptr;

Disassembler::~Disassembler()
{
	delete instance;
}

Instruction Disassembler::disassemble(uint32_t address) {
	uint16_t opcode = memory->read(address);
	OpcodeInfo opcodeInfo = instructionTable[opcode];
	Instruction instruction;

	instruction.address = address;
	instruction.opcode = opcode;
	instruction.operand1 = memory->read(address + 1);
	instruction.operand2 = memory->read(address + 2);
	instruction.mnemonic = opcodeInfo.mnemonic;
	instruction.addressingMode = opcodeInfo.addressingMode;
	instruction.length = opcodeInfo.bytes;

    switch (instruction.addressingMode)
    {
    case kAbsolute:
    case kImmediate16:
    case kAbsoluteIndexedX:
    case kAbsoluteIndexedY:
    case kAbsoluteIndirect:
    case kAbsoluteIndexedIndirectX:
    case kBlockMove:
    case kRelativeLong:
        instruction.bytes[0] = memory->read(address + 1);
        instruction.bytes[1] = memory->read(address + 2);
        instruction.bytes[2] = ' ';
        break;
    case kImmediate8:
    case kImmediate:
    case kAccumulator:
    case kBranch:
    case kDirectPage:
    case kDirectPageIndexedX:
    case kDirectPageIndexedY:
    case kDirectPageIndirect:
    case kDirectPageIndirectLong:
    case kDirectPageIndirectX:
    case kDirectPageIndirectIndexedY:
    case kDirectPageIndirectLongIndexedY:
    case kStackRelative:
    case kStackRelativeIndirectIndexedY:
    case kRelative:
        instruction.bytes[0] = memory->read(address + 1);
        instruction.bytes[1] = ' ';
        instruction.bytes[2] = ' ';
        break;
    case kAbsoluteLong:
    case kAbsoluteLongIndexedX:
    case kAbsoluteIndirectLong:
        instruction.bytes[0] = memory->read(address + 1);
        instruction.bytes[1] = memory->read(address + 2);
        instruction.bytes[2] = memory->read(address + 3);
        break;
    case kNone:
        if (instruction.opcode == 0x42) {
            instruction.bytes[0] = memory->read(address + 1);
            instruction.bytes[1] = ' ';
            instruction.bytes[2] = ' ';
        }
        else {
            instruction.bytes[0] = ' ';
            instruction.bytes[1] = ' ';
            instruction.bytes[2] = ' ';
        }
        break;
    default:
        throw std::runtime_error("Unknown addressing mode.");
        break;
    }

    return instruction;
}