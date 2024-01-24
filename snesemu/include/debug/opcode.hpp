#ifndef OPCODE_HPP
#define OPCODE_HPP

#include "addressing_mode.hpp"

struct OpcodeInfo
{
    const char* mnemonic;
    uint8_t opcode;
    AddressingMode addressingMode;
    int bytes;
};

#endif