#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>

class Cpu
{
public:

private:
	uint8_t E_FLAG;

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
	uint32_t ADDR_getImm_8();
	uint32_t ADDR_getImm_16();
	uint32_t ADDR_getAbs();
	uint32_t ADDR_getAbsLong();
	uint32_t ADDR_getAbsIndexedX();
	uint32_t ADDR_getAbsIndexedY();
	uint32_t ADDR_getAbsLongIndexedX();
	uint32_t ADDR_getAbsIndirect();
	uint32_t ADDR_getAbsIndirectLong();
	uint32_t ADDR_getAbsIndexedIndirectX();
	uint32_t ADDR_getLong();
	uint32_t ADDR_getDirectPage();
	uint32_t ADDR_getDirectPageIndexedX();
	uint32_t ADDR_getDirectPageIndexedY();
	uint32_t ADDR_getDirectPageIndirect();
	uint32_t ADDR_getDirectPageIndirectLong();
	uint32_t ADDR_getDirectPageIndirectX();
	uint32_t ADDR_getDirectPageIndirectIndexedY();
	uint32_t ADDR_getDirectPageIndirectLongIndexedY();
	uint32_t ADDR_getStackRelative();

};

#endif