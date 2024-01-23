#ifndef CPU_HPP
#define CPU_HPP

#include <stdint.h>
#include "memory.hpp"
#include "interrupt.hpp"

class Cpu
{
public:
	Cpu(Memory* memory) {
		regs.SP = INITIAL_STACK_POINTER;
	}

private:
	bool E; // Emulation mode
	static constexpr uint16_t INITIAL_STACK_POINTER = 0x01FF;
	bool pbr;
	bool wai;
	bool stopped;
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
		} P;
	} regs;

	// Stack
	void push(uint8_t value) {
		memory.write(regs.SP, value);
		regs.SP--;
	}

	uint8_t pull() {
		regs.SP++;
		return memory.read(regs.SP);
	}

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
	uint8_t INC(uint8_t cycles);
	uint8_t INC(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PHP(uint8_t cycles);
	uint8_t TCS(uint8_t cycles);
	uint8_t ASL(uint32_t(Cpu::*f)(), uint8_t cycles);
	uint8_t ASL(uint8_t cycles);
	uint8_t PHD(uint8_t cycles);
	uint8_t BPL(uint8_t cycles);
	uint8_t TRB(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t CLC(uint8_t cycles);
	uint8_t JSR(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t AND(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t JSL(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t BIT(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t TXA(uint8_t cycles);
	uint8_t PHB(uint8_t cycles);
	uint8_t BIT_Imm(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t ROL(uint8_t cycles);
	uint8_t PLD(uint8_t cycles);
	uint8_t ROL(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PLP(uint8_t cycles);
	uint8_t BMI(uint8_t cycles);
	uint8_t SEC(uint8_t cycles);
	uint8_t RTI(uint8_t cycles);
	uint8_t EOR(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PHY(uint8_t cycles);
	uint8_t TCD(uint8_t cycles);
	uint8_t JML(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t WDM(uint8_t cycles);
	uint8_t MVP(uint8_t cycles);
	uint8_t LSR(uint8_t cycles);
	uint8_t PHK(uint8_t cycles);
	uint8_t JMP(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t LSR(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PHA(uint8_t cycles);
	uint8_t BVC(uint8_t cycles);
	uint8_t MVN(uint8_t cycles);
	uint8_t CLI(uint8_t cycles);
	uint8_t RTS(uint8_t cycles);
	uint8_t ADC(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PLY(uint8_t cycles);
	uint8_t TDC(uint8_t cycles);
	uint8_t PER(uint8_t cycles);
	uint8_t STZ(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t ROR(uint8_t cycles);
	uint8_t RTL(uint8_t cycles);
	uint8_t ROR(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PLA(uint8_t cycles);
	uint8_t BVS(uint8_t cycles);
	uint8_t SEI(uint8_t cycles);
	uint8_t BRA(uint8_t cycles);
	uint8_t STA(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t BRL(uint8_t cycles);
	uint8_t STY(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t STX(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t DEY(uint8_t cycles);
	uint8_t BCC(uint8_t cycles);
	uint8_t TYA(uint8_t cycles);
	uint8_t TXS(uint8_t cycles);
	uint8_t TXY(uint8_t cycles);
	uint8_t LDY(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t LDA(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t LDX(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t TAY(uint8_t cycles);
	uint8_t TAX(uint8_t cycles);
	uint8_t PLB(uint8_t cycles);
	uint8_t BCS(uint8_t cycles);
	uint8_t CLV(uint8_t cycles);
	uint8_t TSX(uint8_t cycles);
	uint8_t TYX(uint8_t cycles);
	uint8_t CPY(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t CMP(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t CPX(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t SED(uint8_t cycles);
	uint8_t SBC(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PLX(uint8_t cycles);
	uint8_t XCE(uint8_t cycles);
	uint8_t PEA(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t BEQ(uint8_t cycles);
	uint8_t NOP(uint8_t cycles);
	uint8_t XBA(uint8_t cycles);
	uint8_t INX(uint8_t cycles);
	uint8_t SEP(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t PHX(uint8_t cycles);
	uint8_t STP(uint8_t cycles);
	uint8_t CLD(uint8_t cycles);
	uint8_t REP(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t DEC(uint8_t cycles);
	uint8_t TSC(uint8_t cycles);
	uint8_t DEC(uint32_t(Cpu::* f)(), uint8_t cycles);
	uint8_t INY(uint8_t cycles);
	uint8_t DEX(uint8_t cycles);
	uint8_t WAI(uint8_t cycles);
	uint8_t BNE(uint8_t cycles);
	uint8_t PEI(uint8_t cycles);
};

#endif