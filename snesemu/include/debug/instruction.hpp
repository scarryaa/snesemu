#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <stdint.h>
#include "addressing_mode.hpp"

typedef struct {
    uint32_t address;
    uint8_t opcode;
    uint16_t operand1;
    uint16_t operand2;
    uint8_t bytes[3];
    uint16_t length;
    const char* mnemonic;
    AddressingMode addressingMode;
} Instruction;

#endif