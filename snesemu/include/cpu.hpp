#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>
#include "memory.hpp"
#include "interrupt.hpp"

class Cpu
{
public:
	Cpu(Memory* memory) {}

private:
	bool pbr;
	Memory memory;
	Interrupts interrupt;

	// Registers
	struct {
		uint16_t SP;
		uint16_t X;
		uint16_t Y;
		uint16_t A;
		uint16_t D;
		uint8_t PBR;
		uint8_t DBR;
		uint16_t PC;
	      
		// P flags NVMXDIZC
		struct {
			uint8_t C;
			uint8_t Z;
			uint8_t I;
			uint8_t D;
			uint8_t X;
			uint8_t M;
			uint8_t V;
			uint8_t N;
			uint8_t B;
			uint8_t E;
		} P;
	} regs;

	// Addressing Modes
	uint32_t getImm_8();
	uint32_t getImm_16();
	uint32_t getAbs();
	uint32_t getAbsLong();
	uint32_t getAbsIndexedX();
	uint32_t getAbsIndexedY();
	uint32_t getAbsLongIndexedX();
	uint32_t getAbsIndirect();
	uint32_t getAbsIndirectLong();
	uint32_t getAbsIndexedIndirectX();
	uint32_t getLong();
	uint32_t getDirectPage();
	uint32_t getDirectPageIndexedX();
	uint32_t getDirectPageIndexedY();
	uint32_t getDirectPageIndirect();
	uint32_t getDirectPageIndirectLong();
	uint32_t getDirectPageIndirectX();
	uint32_t getDirectPageIndirectIndexedY();
	uint32_t getDirectPageIndirectLongIndexedY();
	uint32_t getStackRelative();
	uint32_t getStackRelativeIndirectIndexedY();

	// Instructions
	uint8_t step();
	uint8_t BRK(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t COP(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t TSB(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t ORA(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t PHP(uint8_t cycles);
	uint8_t ASL(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t ASL(uint8_t cycles);
};

#endif