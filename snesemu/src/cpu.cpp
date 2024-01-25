#include "../include/cpu.hpp"

uint32_t Cpu::getImm_8()
{
	return (regs.PBR << 16) | regs.PC++;
}

uint32_t Cpu::getImm_16()
{
	regs.PC += 2;
	return (regs.PBR << 16) | regs.PC - 2;
}

uint32_t Cpu::getAbs()
{
	uint32_t address = (regs.PBR << 16) | regs.PC;
	uint32_t addr = (memory->read(address + 1) << 8) | memory->read(address);
	regs.PC += 2;
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsLong()
{
	uint32_t address = (regs.PBR << 16) | regs.PC;
	uint32_t value = (memory->read(address + 2) << 16) |
		(memory->read(address + 1) << 8) |
		memory->read(address);
	regs.PC += 3;
	return value;
}

uint32_t Cpu::getAbsIndexedX()
{
	regs.PC += 2;
	uint16_t addr = ((memory->read((regs.PBR << 16) | regs.PC) <<
		8) | memory->read((regs.PBR << 16) | regs.PC - 1));
	pbr = (addr & 0xFF00) != ((addr + regs.X) & 0xFF00);
	addr += regs.X;
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsIndexedY() {
	uint16_t lowByte = memory->read((regs.PBR << 16) | regs.PC);
	regs.PC++;
	uint16_t highByte = memory->read((regs.PBR << 16) | regs.PC);
	regs.PC++;

	uint16_t addr = (highByte << 8) | lowByte;
	pbr = (addr & 0xFF00) != ((addr + regs.Y) & 0xFF00);
	addr += regs.Y;

	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsLongIndexedX()
{
	regs.PC += 3;
	return ((memory->read((regs.PBR << 16) | regs.PC) <<
		16) | (memory->read((regs.PBR << 16) | regs.PC - 1) << 8) |
		memory->read((regs.PBR << 16) | regs.PC - 2)) + regs.X;
}

uint32_t Cpu::getAbsIndirect()
{
	regs.PC += 2;
	uint8_t lo = memory->read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory->read((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory->read(addr);
	uint8_t i_hi = memory->read(addr + 1);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::getAbsIndirectLong()
{
	regs.PC += 2;
	uint8_t lo = memory->read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory->read((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory->read(addr);
	uint8_t i_hi = memory->read(addr + 1);
	regs.PBR = memory->read(addr + 2);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::getAbsIndexedIndirectX()
{
	regs.PC += 2;
	uint8_t lo = memory->read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory->read((regs.PBR << 16) | regs.PC);
	uint32_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory->read(addr);
	uint8_t i_hi = memory->read(addr + 1);
	return (regs.PBR << 16) | (i_hi) << 8 | i_lo;
}

uint32_t Cpu::getLong()
{
	regs.PC += 3;
	return (memory->read((regs.PBR << 16) | regs.PC) << 16) |
		(memory->read((regs.PBR << 16) | regs.PC - 1) << 8) |
		memory->read((regs.PBR << 16) | regs.PC - 2);
}

uint32_t Cpu::getDirectPage()
{
	regs.PC += 1;
	return memory->read((regs.PBR << 16) | regs.PC) + regs.D;
}

uint32_t Cpu::getDirectPageIndexedX()
{
	regs.PC++;
	return memory->read((regs.PBR << 16) | regs.PC) + regs.X;
}

uint32_t Cpu::getDirectPageIndexedY()
{
	regs.PC++;
	return memory->read((regs.PBR << 16) | regs.PC) + regs.Y;
}

uint32_t Cpu::getDirectPageIndirect()
{
	regs.PC++;
	uint8_t dp_index = memory->read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory->read(dp_index + 2) << 16) 
		| (memory->read(dp_index));
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::getDirectPageIndirectLong()
{
	regs.PC++;
	uint8_t dp_index = memory->read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory->read(dp_index + 2) << 16) |
		(memory->read(dp_index + 1) << 8) | memory->read(dp_index);
	return dp_addr;
}

uint32_t Cpu::getDirectPageIndirectX()
{
	regs.PC++;
	uint8_t dp_index = memory->read(((regs.PBR << 16) | regs.PC) + regs.D) + regs.X;
	uint16_t dp_addr = (memory->read(dp_index + 1) << 8) |
		memory->read(dp_index);
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::getDirectPageIndirectIndexedY()
{
	regs.PC++;
	uint8_t dp_index = memory->read(((regs.PBR << 16) |
		regs.PC)) + regs.D;
	uint32_t dp_addr = (regs.DBR << 16) | (memory->read(dp_index + 1) << 8) | 
		memory->read(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::getDirectPageIndirectLongIndexedY()
{
	regs.PC++;
	uint8_t dp_index = memory->read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory->read(dp_index + 2) << 16) |
		(memory->read(dp_index + 1) << 8) |
		memory->read(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::getStackRelative()
{
	regs.PC++;
	uint8_t byte = memory->read((regs.PBR << 16) | regs.PC);
	return regs.SP + byte;
}

uint32_t Cpu::getStackRelativeIndirectIndexedY()
{
	regs.PC++;
	uint8_t byte = memory->read((regs.PBR << 16) | regs.PC);
	uint8_t base = memory->read(((regs.DBR << 16) | regs.SP) + byte);
	return base + regs.Y;
}

void Cpu::reset() {
	interrupt = Interrupts::NONE;
	regs.SP = INITIAL_STACK_POINTER;
	regs.P.C = false;
	regs.P.D = false;
	regs.P.I = true;
	regs.P.M = true;
	regs.P.N = false;
	regs.P.V = false;
	regs.P.X = true;
	regs.P.Z = false;
	B = true;
	E = true;

	regs.A = 0;
	regs.D = 0;
	regs.DBR = 0;
	regs.PBR = 0;
	regs.X = 0;
	regs.Y = 0;
}

uint8_t Cpu::step() {
	uint8_t opcode = memory->read(regs.PC++);
	uint8_t mFlagInv = !regs.P.M ? 1 : 0;
	uint8_t mFlagInv_2 = !regs.P.M ? 2 : 0;
	uint8_t dpL = (regs.D & 0x00FF) ? 1 : 0;
	uint8_t xFlagInv = !regs.P.X ? 1 : 0;

	uint8_t irq_cycles = 0;

	// Check for interrupts
	if (interrupt != Interrupts::NONE && regs.P.I != 1)
	{
		irq_cycles = Interrupt::handle_interrupt(interrupt, this, memory);
		this->interrupt = Interrupts::NONE;
		return irq_cycles;
	}

	while (stopped || (wai && regs.P.I == 1)) {
		// Wait for interrupt to wake up
		if (interrupt != Interrupts::NONE) {
			stopped = false;
			wai = false;
			break;
		}
	}

	// Notes
	// M,X = 1 8-bit (immediate only) +1 cycle
	// M,X = 0 16-bit (immediate only)
	switch (opcode)
	{
	case 0x00: return BRK(&Cpu::getImm_8, 0); // return 0 since we return 7 + !E in the interrupt method
	case 0x01: return ORA(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x02: return COP(&Cpu::getImm_8, 0); // return 0, same as above
	case 0x03: return ORA(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x04: return TSB(&Cpu::getDirectPage, 5 + mFlagInv_2 + dpL);
	case 0x05: return ORA(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x06: return ASL(&Cpu::getDirectPage, 5 + mFlagInv_2 + dpL);
	case 0x07: return ORA(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x08: return PHP(3);
	case 0x09: return (regs.P.M) ? ORA(&Cpu::getImm_8, 2) : ORA(&Cpu::getImm_16, 3);
	case 0x0A: return ASL(2);
	case 0x0B: return PHD(4);
	case 0x0C: return TSB(&Cpu::getAbs, 6 + mFlagInv_2);
	case 0x0D: return ORA(&Cpu::getAbs, 4 + mFlagInv);
	case 0x0E: return ASL(&Cpu::getAbs, 6 + mFlagInv_2);
	case 0x0F: return ORA(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0x10: return BPL(2 + E ? 1 : 0);
	case 0x11: return ORA(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr ? 1 : 0);
	case 0x12: return ORA(&Cpu::getDirectPageIndirect, 7 + mFlagInv + dpL);
	case 0x13: return ORA(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x14: return TRB(&Cpu::getDirectPage, 5 + mFlagInv_2 + dpL);
	case 0x15: return ORA(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x16: return ASL(&Cpu::getDirectPageIndexedX, 6 + mFlagInv_2 + dpL);
	case 0x17: return ORA(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x18: return CLC(2);
	case 0x19: return ORA(&Cpu::getAbsIndexedY, 4 + mFlagInv + (pbr ? 1 : 0));
	case 0x1A: return INC(2);
	case 0x1B: return TCS(2);
	case 0x1C: return TRB(&Cpu::getAbs, 6 + mFlagInv_2);
	case 0x1D: return ORA(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0x1E: return ASL(&Cpu::getAbsIndexedX, 7 + mFlagInv_2 + pbr);
	case 0x1F: return ORA(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0x20: return JSR(&Cpu::getAbs, 6);
	case 0x21: return AND(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x22: return JSL(&Cpu::getAbsLong, 8);
	case 0x23: return AND(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x24: return BIT(&Cpu::getDirectPage, 3 + mFlagInv + dpL, false);
	case 0x25: return AND(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x26: return ROL(&Cpu::getDirectPage, 5 + mFlagInv + dpL);
	case 0x27: return AND(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x28: return PLP(4);
	case 0x29: return (regs.P.M) ? AND(&Cpu::getImm_8, 2) : AND(&Cpu::getImm_16, 3);
	case 0x2A: return ROL(2);
	case 0x2B: return PLD(5);
	case 0x2C: return BIT(&Cpu::getAbs, 4 + mFlagInv, false);
	case 0x2D: return AND(&Cpu::getAbs, 4 + mFlagInv);
	case 0x2E: return ROL(&Cpu::getAbs, 6 + mFlagInv);
	case 0x2F: return AND(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0x30: return BMI(2 + E ? 1 : 0);
	case 0x31: return AND(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL);
	case 0x32: return AND(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0x33: return AND(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x34: return BIT(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL, false);
	case 0x35: return AND(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x36: return ROL(&Cpu::getDirectPageIndexedX, 6 + mFlagInv + dpL);
	case 0x37: return AND(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x38: return SEC(2);
	case 0x39: return AND(&Cpu::getAbsIndexedY, 4 + mFlagInv + (pbr ? 1 : 0));
	case 0x3A: return DEC(2);
	case 0x3B: return TSC(2);
	case 0x3C: return BIT(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr, false);
	case 0x3D: return AND(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0x3E: return ROL(&Cpu::getAbsIndexedX, 7 + mFlagInv + pbr);
	case 0x3F: return AND(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0x40: return RTI(6 + !E);
	case 0x41: return EOR(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x42: return WDM(2);
	case 0x43: return EOR(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x44: return MVP(7);
	case 0x45: return EOR(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x46: return LSR(&Cpu::getDirectPage, 5 + mFlagInv + dpL);
	case 0x47: return EOR(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x48: return PHA(3 + mFlagInv);
	case 0x49: return (regs.P.M) ? EOR(&Cpu::getImm_8, 2) : EOR(&Cpu::getImm_16, 3);
	case 0x4A: return LSR(2);
	case 0x4B: return PHK(3);
	case 0x4C: return JMP(&Cpu::getAbs, 3);
	case 0x4D: return EOR(&Cpu::getAbs, 4 + mFlagInv);
	case 0x4E: return LSR(&Cpu::getAbs, 6 + mFlagInv);
	case 0x4F: return EOR(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0x50: return BVC(2 + E ? 1 : 0);
	case 0x51: return EOR(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr);
	case 0x52: return EOR(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0x53: return EOR(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x54: return MVN(7);
	case 0x55: return EOR(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x56: return LSR(&Cpu::getDirectPageIndexedX, 6 + mFlagInv + dpL);
	case 0x57: return EOR(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x58: return CLI(2);
	case 0x59: return EOR(&Cpu::getAbsIndexedY, 4 + mFlagInv + pbr);
	case 0x5A: return PHY(3 + xFlagInv);
	case 0x5B: return TCD(2);
	case 0x5C: return JML(&Cpu::getAbsLong, 4);
	case 0x5D: return EOR(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0x5E: return LSR(&Cpu::getAbsIndexedX, 7 + mFlagInv + pbr);
	case 0x5F: return EOR(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0x60: return RTS(6);
	case 0x61: return ADC(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x62: return PER(6);
	case 0x63: return ADC(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x64: return STZ(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x65: return ADC(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x66: return ROR(&Cpu::getDirectPage, 5 + mFlagInv + dpL);
	case 0x67: return ADC(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x68: return PLA(4 + mFlagInv);
	case 0x69: return (regs.P.M) ? ADC(&Cpu::getImm_8, 2) : ADC(&Cpu::getImm_16, 3);
	case 0x6A: return ROR(2);
	case 0x6B: return RTL(6);
	case 0x6C: return JMP(&Cpu::getAbsIndirect, 5);
	case 0x6D: return ADC(&Cpu::getAbs, 4 + mFlagInv);
	case 0x6E: return ROR(&Cpu::getAbs, 6 + mFlagInv);
	case 0x6F: return ADC(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0x70: return BVS(2 + E ? 1 : 0);
	case 0x71: return ADC(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr);
	case 0x72: return ADC(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0x73: return ADC(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x74: return STZ(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x75: return ADC(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x76: return ROR(&Cpu::getDirectPageIndexedX, 6 + mFlagInv + dpL);
	case 0x77: return ADC(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x78: return SEI(2);
	case 0x79: return ADC(&Cpu::getAbsIndexedY, 4 + mFlagInv + pbr);
	case 0x7A: return PLY(4 + xFlagInv);
	case 0x7B: return TDC(2);
	case 0x7C: return JMP(&Cpu::getAbsIndexedIndirectX, 6);
	case 0x7D: return ADC(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0x7E: return ROR(&Cpu::getAbsIndexedX, 7 + mFlagInv + pbr);
	case 0x7F: return ADC(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0x80: return BRA(2 + E ? 1 : 0);
	case 0x81: return STA(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x82: return BRL(3);
	case 0x83: return STA(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x84: return STY(&Cpu::getDirectPage, 3 + (xFlagInv) + dpL);
	case 0x85: return STA(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x86: return STX(&Cpu::getDirectPage, 3 + (xFlagInv) + dpL);
	case 0x87: return STA(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x88: return DEY(2);
	case 0x89: return (regs.P.M) ? BIT(&Cpu::getImm_8, 2, true) : BIT(&Cpu::getImm_16, 3, true);
	case 0x8A: return TXA(3);
	case 0x8B: return PHB(3);
	case 0x8C: return STY(&Cpu::getAbs, 4 + xFlagInv);
	case 0x8D: return STA(&Cpu::getAbs, 4 + mFlagInv);
	case 0x8E: return STX(&Cpu::getAbs, 4 + xFlagInv);
	case 0x8F: return STA(&Cpu::getAbsLong, 4 + mFlagInv);
	case 0x90: return BCC(2 + E ? 1 : 0);
	case 0x91: return STA(&Cpu::getDirectPageIndirectIndexedY, 6 + mFlagInv + dpL);
	case 0x92: return STA(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0x93: return STA(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x94: return STY(&Cpu::getDirectPageIndexedX, 4 + !regs.P.X + dpL);
	case 0x95: return STA(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x96: return STX(&Cpu::getDirectPageIndexedY, 4 + !regs.P.X + dpL);
	case 0x97: return STA(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x98: return TYA(2);
	case 0x99: return STA(&Cpu::getAbsIndexedY, 5 + mFlagInv);
	case 0x9A: return TXS(2);
	case 0x9B: return TXY(2);
	case 0x9C: return STZ(&Cpu::getAbs, 4 + mFlagInv);
	case 0x9D: return STA(&Cpu::getAbsIndexedX, 5 + mFlagInv);
	case 0x9E: return STZ(&Cpu::getAbsIndexedX, 5 + mFlagInv);
	case 0x9F: return STA(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0xA0: return (regs.P.X) ? LDY(&Cpu::getImm_8, 2) : LDY(&Cpu::getImm_16, 3);
	case 0xA1: return LDA(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0xA2: return (regs.P.X) ? LDX(&Cpu::getImm_8, 2) : LDX(&Cpu::getImm_16, 3);
	case 0xA3: return LDA(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0xA4: return LDY(&Cpu::getDirectPage, 3 + (xFlagInv));
	case 0xA5: return LDA(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0xA6: return LDX(&Cpu::getDirectPage, 3 + (xFlagInv) + dpL);
	case 0xA7: return LDA(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0xA8: return TAY(2);
	case 0xA9: return (regs.P.M) ? LDA(&Cpu::getImm_8, 2) : LDA(&Cpu::getImm_16, 3);
	case 0xAA: return TAX(2);
	case 0xAB: return PLB(4);
	case 0xAC: return LDY(&Cpu::getAbs, 4 + (xFlagInv));
	case 0xAD: return LDA(&Cpu::getAbs, 4 + mFlagInv);
	case 0xAE: return LDX(&Cpu::getAbs, 4 + (!regs.X ? 1 : 0));
	case 0xAF: return LDA(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0xB0: return BCS(2 + E ? 1 : 0);
	case 0xB1: return LDA(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr);
	case 0xB2: return LDA(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0xB3: return LDA(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0xB4: return LDY(&Cpu::getDirectPageIndexedX, 4 + (xFlagInv) + dpL);
	case 0xB5: return LDA(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0xB6: return LDX(&Cpu::getDirectPageIndexedY, 4 + xFlagInv);
	case 0xB7: return LDA(&Cpu::getDirectPageIndirectLongIndexedY, 2 + mFlagInv + dpL);
	case 0xB8: return CLV(2);
	case 0xB9: return LDA(&Cpu::getAbsIndexedY, 4 + mFlagInv + pbr);
	case 0xBA: return TSX(2);
	case 0xBB: return TYX(2);
	case 0xBC: return LDY(&Cpu::getAbsIndexedX, 4 + xFlagInv + pbr);
	case 0xBD: return LDA(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0xBE: return LDX(&Cpu::getAbsIndexedY, 4 + xFlagInv + pbr);
	case 0xBF: return LDA(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0xC0: return (regs.P.X) ? CPY(&Cpu::getImm_8, 2) : CPY(&Cpu::getImm_16, 3);
	case 0xC1: return CMP(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0xC2: return REP(&Cpu::getImm_8, 3);
	case 0xC3: return CMP(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0xC4: return CPY(&Cpu::getDirectPage, 3 + xFlagInv + dpL);
	case 0xC5: return CMP(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0xC6: return DEC(&Cpu::getDirectPage, 5 + mFlagInv_2 + dpL);
	case 0xC7: return CMP(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0xC8: return INY(2);
	case 0xC9: return (regs.P.M) ? CMP(&Cpu::getImm_8, 2) : CMP(&Cpu::getImm_16, 3);
	case 0xCA: return DEX(2);
	case 0xCB: return WAI(3);
	case 0xCC: return CPY(&Cpu::getAbs, 2 + xFlagInv);
	case 0xCD: return CMP(&Cpu::getAbs, 4 + mFlagInv);
	case 0xCE: return DEC(&Cpu::getAbs, 6 + mFlagInv_2);
	case 0xCF: return CMP(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0xD0: return BNE(2 + E ? 1 : 0);
	case 0xD1: return CMP(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr);
	case 0xD2: return CMP(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0xD3: return CMP(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0xD4: return PEI(6 + dpL);
	case 0xD5: return CMP(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0xD6: return DEC(&Cpu::getDirectPageIndexedX, 6 + mFlagInv_2 + dpL);
	case 0xD7: return CMP(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0xD8: return CLD(2);
	case 0xD9: return CMP(&Cpu::getAbsIndexedY, 4 + mFlagInv + pbr);
	case 0xDA: return PHX(3);
	case 0xDB: return STP(3);
	case 0xDC: return JML(&Cpu::getAbsIndirectLong, 6);
	case 0xDD: return CMP(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0xDE: return DEC(&Cpu::getAbsIndexedX, 7 + mFlagInv_2 + pbr);
	case 0xDF: return CMP(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	case 0xE0: return (regs.P.X) ? CPX(&Cpu::getImm_8, 2) : CPX(&Cpu::getImm_16, 3);
	case 0xE1: return SBC(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0xE2: return SEP(&Cpu::getImm_8 ,3);
	case 0xE3: return SBC(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0xE4: return CPX(&Cpu::getDirectPage, 3 + xFlagInv + dpL);
	case 0xE5: return SBC(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0xE6: return INC(&Cpu::getDirectPage, 5 + mFlagInv_2 + dpL);
	case 0xE7: return SBC(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0xE8: return INX(2);
	case 0xE9: return (regs.P.M) ? SBC(&Cpu::getImm_8, 2) : SBC(&Cpu::getImm_16, 3);
	case 0xEA: return NOP(2);
	case 0xEB: return XBA(3);
	case 0xEC: return CPX(&Cpu::getAbs, xFlagInv);
	case 0xED: return SBC(&Cpu::getAbs, mFlagInv);
	case 0xEE: return INC(&Cpu::getAbs, 6 + mFlagInv_2);
	case 0xEF: return SBC(&Cpu::getAbsLong, 5 + mFlagInv);
	case 0xF0: return BEQ(2 + E ? 1 : 0);
	case 0xF1: return SBC(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL + pbr);
	case 0xF2: return SBC(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0xF3: return SBC(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0xF4: return PEA(&Cpu::getAbs, 5);
	case 0xF5: return SBC(&Cpu::getDirectPageIndirectX, 4 + mFlagInv + dpL);
	case 0xF6: return INC(&Cpu::getDirectPageIndexedX, 6 + mFlagInv_2 + dpL);
	case 0xF7: return SBC(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0xF8: return SED(2);
	case 0xF9: return SBC(&Cpu::getAbsIndexedY, 4 + mFlagInv + pbr);
	case 0xFA: return PLX(4);
	case 0xFB: return XCE(2);
	case 0xFC: return JSR(&Cpu::getAbsIndexedIndirectX, 8);
	case 0xFD: return SBC(&Cpu::getAbsIndexedX, 4 + mFlagInv + pbr);
	case 0xFE: return INC(&Cpu::getAbsIndexedX, 7 + mFlagInv_2 + pbr);
	case 0xFF: return SBC(&Cpu::getAbsLongIndexedX, 5 + mFlagInv);
	}

	return 0;
}

uint8_t Cpu::BRK(uint32_t(Cpu::*f)(), uint8_t cycles) {
	// set BRK interrupt
	B = true;
	regs.P.I = true;
	regs.P.D = false;
	interrupt = Interrupts::BRK;
	// the rest is handled in the interrupt handling method

	return cycles;
}

uint8_t Cpu::COP(uint32_t(Cpu::* f)(), uint8_t cycles) {
	// set COP interrupt
	regs.P.I = 1;
	regs.P.D = 0;
	interrupt = Interrupts::COP;
	// the rest is handled in the interrupt handling method

	return cycles;
}

uint8_t Cpu::TSB(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint16_t val;
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		val = memory->read(addr);
		regs.P.Z = (regs.A & val & 0xFF) == 0;
		memory->write(addr, val | (regs.A & 0xFF));
	}
	else {
		// 16-bit mode
		val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.Z = (regs.A & val) == 0;
		memory->write(addr, val | (regs.A & 0xFF));
		memory->write(addr + 1, (val >> 8) | (regs.A >> 8));
	}

	return cycles;
}


uint8_t Cpu::ORA(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.A = (regs.A & 0xFF00) | ((regs.A & 0x00FF) | val);

		regs.P.N = regs.A & 0x0080 ? 1 : 0;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.A |= val;

		regs.P.N = regs.A & 0x8000 ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

// accumulator only
uint8_t Cpu::INC(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.A = (regs.A & 0xFF00) | ((regs.A + 1) & 0x00FF);

		regs.P.N = regs.A & 0x80;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		regs.A++;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}
	return cycles;
}

uint8_t Cpu::TCS(uint8_t cycles) {
	regs.SP = regs.A;

	return cycles;
}

uint8_t Cpu::INC(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read(addr);
		val++;
		memory->write(addr, val);

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint16_t val = memory->read(addr) | memory->read(addr + 1) << 8;
		val++;
		memory->write(addr, val);

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
	}

	return cycles;
}

// only affects accumulator
uint8_t Cpu::ASL(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.P.C = (regs.A & 0x80) >> 7;
		regs.A = (regs.A << 1) & 0xFF;

		regs.P.N = (regs.A & 0x80) ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}
	else {
		// 16-bit mode
		regs.P.C = (regs.A & 0x8000) >> 15;
		regs.A <<= 1;

		regs.P.N = (regs.A & 0x8000) ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::PHD(uint8_t cycles) {
	push((regs.D >> 8) & 0xFF);
	push(regs.D & 0xFF);

	return cycles;
}

// affects memory
uint8_t Cpu::ASL(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read(addr);
		regs.P.C = (val & 0x80) >> 7;
		val <<= 1;

		memory->write(addr, val);

		regs.P.N = (val & 0x80) ? 1 : 0;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.C = (val & 0x8000) >> 15;
		val <<= 1;

		memory->write(addr, val & 0xFF);
		memory->write(addr + 1, val >> 8);

		regs.P.N = (val & 0x8000) ? 1 : 0;
		regs.P.Z = val == 0;
	}

	return cycles;
}

uint8_t Cpu::PHP(uint8_t cycles) {
	uint8_t status = 0;
	status |= regs.P.C;
	status |= regs.P.Z << 1;
	status |= regs.P.I << 2;
	status |= regs.P.D << 3;
	status |= regs.P.X << 4;
	status |= regs.P.M << 5;
	status |= regs.P.V << 6;
	status |= regs.P.N << 7;

	status |= (1 << 4); // Set B flag in bit 4
	status |= E << 5; // Set E flag in bit 5

	push(status);
	return cycles;
}

uint8_t Cpu::BPL(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (!regs.P.N) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::TRB(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read(addr);
		regs.P.Z = (regs.A & val & 0xFF) == 0;
		uint8_t result = val & ~(regs.A & 0xFF);
		memory->write(addr, result);
	}
	else {
		// 16-bit mode
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.Z = (regs.A & val) == 0;
		uint16_t result = val & ~regs.A;
		memory->write(addr, result & 0xFF);
		memory->write(addr + 1, result >> 8);
	}

	return cycles;
}

uint8_t Cpu::CLC(uint8_t cycles) {
	// Clear carry flag
	regs.P.C = 0;

	return cycles;
}

uint8_t Cpu::JSR(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	// The return address is the address of the byte before the next instruction
	uint16_t returnAddress = regs.PC + 2;

	// Push the high byte and then the low byte of the return address onto the stack
	push((returnAddress >> 8) & 0xFF);
	push(returnAddress & 0xFF);

	regs.PC = addr;

	return cycles;
}

uint8_t Cpu::AND(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.A = (regs.A & 0xFF00) | ((regs.A & 0x00FF) & val);

		regs.P.N = regs.A & 0x0080 ? 1 : 0;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.A &= val;

		regs.P.N = regs.A & 0x8000 ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::JSL(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	// The return address is the address immediately after the JSL instruction
	uint32_t returnAddress = (regs.PBR << 16) | (regs.PC + 3);

	// Push the return address onto the stack (PBR, then high byte of PC, then low byte of PC)
	push((returnAddress >> 16) & 0xFF);
	push((returnAddress >> 8) & 0xFF);
	push(returnAddress & 0xFF);

	regs.PBR = (addr >> 16) & 0xFF;
	regs.PC = addr & 0xFFFF;

	return cycles;
}

uint8_t Cpu::BIT(uint32_t(Cpu::* f)(), uint8_t cycles, bool isImmediate) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		printf("BIT %04X ", val);

		if (!isImmediate) {
			regs.P.N = (val >> 7);
			regs.P.V = ((val >> 6) & 1);
		}
		regs.P.Z = (val & (regs.A & 0x00FF)) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);

		if (!isImmediate) {
			regs.P.N = val >> 15;
			regs.P.V = ((val >> 14) & 1);
		}
		regs.P.Z = (val & regs.A) == 0;
	}

	return cycles;
}

uint8_t Cpu::TXA(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.A = (regs.A & 0xFF00) | (regs.X & 0x00FF);

		regs.P.N = regs.A & 0x0080;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		regs.A = regs.X;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::PHB(uint8_t cycles) {
	push(regs.DBR);

	return cycles;
}

uint8_t Cpu::BIT_Imm(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint16_t val;

	if (regs.P.M) {
		// 8-bit mode
		val = memory->read((this->*f)());
		uint8_t res = regs.A & val;
		regs.P.Z = (res & 0xFF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint16_t res = regs.A & val;
		regs.P.Z = res == 0;
	}

	return cycles;
}

// accumulator only
uint8_t Cpu::ROL(uint8_t cycles) {
	uint8_t oldC = regs.P.C;

	if (regs.P.M) {
		// 8-bit mode
		regs.P.C = (regs.A & 0x80) >> 7;
		regs.A <<= 1;
		regs.A |= oldC;

		regs.P.N = regs.A & 0x80;
		regs.P.Z = regs.A == 0;
	}
	else {
		// 16-bit mode
		regs.P.C = (regs.A & 0x8000) >> 15;
		regs.A <<= 1;
		regs.A |= oldC;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::ROL(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint8_t oldC = regs.P.C;

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.P.C = (val & 0x80) >> 7;
		val <<= 1;
		val |= oldC;

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.C = (val & 0x8000) >> 15;
		val <<= 1;
		val |= oldC;

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
	}

	return cycles;
}

uint8_t Cpu::PLD(uint8_t cycles) {
	regs.D = pull();
	regs.D |= pull() << 8;

	return cycles;
}



uint8_t Cpu::PLP(uint8_t cycles) {
	// Pull P from stack
	uint8_t P = pull();

	regs.P.C = P & 0x01;
	regs.P.Z = (P >> 1) & 0x01;
	regs.P.I = (P >> 2) & 0x01;
	regs.P.D = (P >> 3) & 0x01;
	regs.P.X = (P >> 4) & 0x01;
	regs.P.M = (P >> 5) & 0x01;
	regs.P.V = (P >> 6) & 0x01;
	regs.P.N = (P >> 7) & 0x01;

	return cycles;
}

uint8_t Cpu::BMI(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (regs.P.N) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::SEC(uint8_t cycles) {
	regs.P.C = 1;

	return cycles;
}

uint8_t Cpu::RTI(uint8_t cycles) {
	// Pull P from stack
	uint8_t P = pull();

	regs.P.C = P & 0x01;
	regs.P.Z = (P >> 1) & 0x01;
	regs.P.I = (P >> 2) & 0x01;
	regs.P.D = (P >> 3) & 0x01;
	regs.P.X = (P >> 4) & 0x01;
	regs.P.M = (P >> 5) & 0x01;
	regs.P.V = (P >> 6) & 0x01;
	regs.P.N = (P >> 7) & 0x01;

	// Pull PC from stack
	regs.PC = pull() | (pull() << 8);

	return cycles;
}

uint8_t Cpu::EOR(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());

		regs.A = (regs.A & 0xFF00) | ((regs.A & 0x00FF) ^ val);

		regs.P.N = regs.A & 0x0080 ? 1 : 0;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.A ^= val;

		regs.P.N = regs.A & 0x8000 ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}
	return cycles;
}

uint8_t Cpu::PHY(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		push(regs.Y & 0x00FF);
	}
	else {
		// 16-bit mode
		push(regs.Y & 0x00FF);
		push(regs.Y & 0xFF00 >> 8);
	}

	return cycles;
}

uint8_t Cpu::TCD(uint8_t cycles) {
	regs.D = regs.A;

	regs.P.N = regs.D == 0x8000;
	regs.P.Z = regs.D == 0;

	return cycles;
}

uint8_t Cpu::JML(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();
	regs.PC = addr;

	return cycles;
}

uint8_t Cpu::WDM(uint8_t cycles) {
	regs.PC++;

	return cycles;
}

// TODO double check this
uint8_t Cpu::MVP(uint8_t cycles) {
	// Read the source bank and destination bank
	uint8_t srcBank = memory->read(regs.PC);
	uint8_t destBank = memory->read(regs.PC + 1);

	regs.DBR = destBank;

	// Read the source address and destination address
	uint32_t srcAddr = (srcBank << 16) | regs.Y;
	uint32_t destAddr = (destBank << 16) | regs.X;

	// Transfer data from source to destination
	uint8_t val = memory->read(srcAddr);
	memory->write(destAddr, val);

	// Increment source and destination addresses
	regs.X++;
	regs.Y++;

	// Decrement the accumulator
	if (regs.A > 0) {
		regs.A--;
		regs.PC -= 2; // Loop back to the beginning of the MVP instruction
	}

	// Increment PC to point to the next instruction
	regs.PC += 2;

	return cycles;
}

// only affects accumulator
uint8_t Cpu::LSR(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.P.C = (regs.A & 0x01);
		regs.A = (regs.A >> 1) & 0xFF;

		regs.P.N = (regs.A & 0x80) ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}
	else {
		// 16-bit mode
		regs.P.C = (regs.A & 0x0001);
		regs.A <<= 1;

		regs.P.N = (regs.A & 0x8000) ? 1 : 0;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::PHK(uint8_t cycles) {
	push(regs.PBR);

	return cycles;
}

uint8_t Cpu::JMP(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();
	regs.PC = addr;

	return cycles;
}

uint8_t Cpu::LSR(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.P.C = val & 0x01;
		val >>= 1;
		val &= 0x7F; // Ensure bit 7 is cleared

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
		memory->write((this->*f)(), val); // Write back the result
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.C = val & 0x0001;
		val >>= 1;
		val &= 0x7FFF; // Ensure bit 15 is cleared (mask with 0x7FFF)

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
		memory->write(addr, val & 0xFF); // Write back the low byte
		memory->write(addr + 1, val >> 8); // Write back the high byte
	}

	return cycles;
}

uint8_t Cpu::PHA(uint8_t cycles) {
	// push accumulator to stack
	if (regs.P.M) {
		// 8-bit mode
		push(regs.A & 0x00FF);
	}
	else {
		// 16-bit mode
		push(regs.A);
		push(regs.A >> 8);
	}

	return cycles;
}

uint8_t Cpu::BVC(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (!regs.P.V) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

// TODO double check this
uint8_t Cpu::MVN(uint8_t cycles) {
	// Read the source bank and destination bank
	uint8_t destBank = memory->read(regs.PC);
	uint8_t srcBank = memory->read(regs.PC + 1);

	regs.DBR = destBank;

	// Read the source address and destination address
	uint32_t srcAddr = (srcBank << 16) | regs.X;
	uint32_t destAddr = (destBank << 16) | regs.Y;

	// Transfer data from source to destination
	uint8_t val = memory->read(srcAddr);
	memory->write(destAddr, val);

	// Increment source and destination addresses
	regs.X++;
	regs.Y++;

	// Decrement the accumulator
	if (regs.A > 0) {
		regs.A--;
		regs.PC -= 2; // Loop back to the beginning of the MVN instruction
	}

	// Increment PC to point to the next instruction
	regs.PC += 2;

	return cycles;
}

uint8_t Cpu::CLI(uint8_t cycles) {
	// Clear I flag
	regs.P.I = 0;

	return cycles;
}

uint8_t Cpu::RTS(uint8_t cycles) {
	// Pull PC
	regs.PC = pull() | pull() << 8;
	regs.PC++;

	return cycles;
}

// https://emudev.de/q00-snes/65816-the-cpu/
uint8_t Cpu::ADC(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		uint32_t res = 0;

		if (regs.P.D) {
			res = (regs.A & 0xF) + (val & 0x0F) + regs.P.C;
			if (res > 0x09) res += 0x06;
			regs.P.C = res > 0x0F;
			res = (regs.A & 0xF0) + (val & 0xF0) + (regs.P.C << 4) + (res & 0x0F);
		}
		else {
			res = (regs.A & 0xFF) + val + regs.P.C;
		}

		regs.P.V = ((~(regs.A ^ val) & (regs.A ^ res) & 0x80) == 0x80);
		if (regs.P.D && res > 0x9F) res += 0x60;
		regs.P.C = res > 0xFF;
		regs.P.Z = (uint8_t)res == 0;
		regs.P.N = (res & 0x80) == 0x80;
		regs.A = ((uint8_t)res & 0xFF);
	}
	else {
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint32_t res = 0;

		if (regs.P.D) {
			res = (regs.A & 0x000F) + (val & 0x000F) + regs.P.C;
			if (res > 0x0009) res += 0x0006;
			regs.P.C = res > 0x000F;
			res = (regs.A & 0x00F0) + (val & 0x00F0) + (regs.P.C << 4) + (res & 0x000F);
			if (res > 0x009F) res += 0x0060;
			regs.P.C = res > 0x00FF;
			res = (regs.A & 0x0F00) + (val & 0x0F00) + (regs.P.C << 8) + (res & 0x00FF);
			if (res > 0x09FF) res += 0x0600;
			regs.P.C = res > 0x0FFF;
			res = (regs.A & 0xF000) + (val & 0xF000) + (regs.P.C << 12) + (res & 0x0FFF);
		}
		else {
			res = regs.A + val + regs.P.C;
		}

		regs.P.V = ((~(regs.A ^ val) & (regs.A ^ res) & 0x8000) == 0x8000);
		if (regs.P.D && res > 0x9FFF) res += 0x6000;
		regs.P.C = res > 0xFFFF;
		regs.P.Z = (uint16_t)res == 0;
		regs.P.N = res & 0x8000 == 0x8000;
		regs.A = (uint16_t)res;
	}

	return cycles;
}

uint8_t Cpu::PLY(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		regs.Y = (regs.Y & 0xFF00) | pull();

		regs.P.N = regs.Y & 0x0080;
		regs.P.Z = regs.Y & 0x00FF == 0;
	}
	else {
		// 16-bit mode
		regs.Y = pull();
		regs.Y |= pull() << 8;

		regs.P.N = regs.Y & 0x8000;
		regs.P.Z = regs.Y == 0;
	}
	return cycles;
}

uint8_t Cpu::TDC(uint8_t cycles) {
	regs.A = regs.D;

	regs.P.N = regs.A == 0x8000;
	regs.P.Z = regs.A == 0;

	return cycles;
}

uint8_t Cpu::PER(uint8_t cycles) {
	uint16_t relAddr = memory->read(regs.PC++) | memory->read(regs.PC++) << 8;
	uint16_t effAddr = regs.PC + relAddr;

	push((effAddr >> 8) & 0xFF);
	push(effAddr & 0xFF);

	return cycles;
}

uint8_t Cpu::STZ(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();
	memory->write(addr, 0); // Always write zero

	return cycles;
}

// accumulator only
uint8_t Cpu::ROR(uint8_t cycles) {
	uint8_t oldC = regs.P.C;

	if (regs.P.M) {
		// 8-bit mode
		regs.P.C = regs.A & 0x01;
		regs.A = (regs.A >> 1) | (oldC << 7);

		regs.P.N = regs.A & 0x80;
		regs.P.Z = regs.A == 0;
	}
	else {
		// 16-bit mode
		regs.P.C = regs.A & 0x0001;
		regs.A = (regs.A >> 1) | (oldC << 15);

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::RTL(uint8_t cycles) {
	// Pull PC
	regs.PC = pull() | pull() << 8;
	regs.PC++;

	// pull PBR
	regs.PBR = pull();

	return cycles;
}

uint8_t Cpu::ROR(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint8_t oldC = regs.P.C;

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.P.C = val & 0x01;
		val = (val >> 1) | (oldC << 7);

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.P.C = val & 0x0001;
		val = (val >> 1) | (oldC << 15);

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
	}

	return cycles;
}

uint8_t Cpu::PLA(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.A = (regs.A & 0xFF00) | pull();

		regs.P.N = regs.A & 0x0080;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		regs.A = pull() | (pull() << 8);

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::BVS(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (regs.P.V) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::SEI(uint8_t cycles) {
	// set interrupt disable
	regs.P.I = 1;

	return cycles;
}

uint8_t Cpu::BRA(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	uint16_t oldPC = regs.PC;
	regs.PC += offset;

	// Check for page crossing
	if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
		cycles++;
	}
	cycles++;

	return cycles;
}

uint8_t Cpu::STA(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		memory->write(addr, regs.A & 0x00FF);
	}
	else {
		// 16-bit mode
		memory->write(addr, regs.A & 0x00FF);
		memory->write(addr + 1, (regs.A >> 8) & 0x00FF);
	}

	return cycles;
}

uint8_t Cpu::BRL(uint8_t cycles) {
	int16_t offset = memory->read(regs.PC++) | (memory->read(regs.PC++) << 8);

	regs.PC += offset;

	// BRL always adds an extra cycle
	cycles++;

	return cycles;
}

uint8_t Cpu::STY(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.X) {
		// 8-bit mode
		memory->write(addr, regs.Y & 0x00FF);
	}
	else {
		// 16-bit mode
		memory->write(addr, regs.Y & 0x00FF);
		memory->write(addr + 1, (regs.Y >> 8) & 0x00FF);
	}

	return cycles;
}

uint8_t Cpu::STX(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.X) {
		// 8-bit mode
		memory->write(addr, regs.X & 0x00FF);
	}
	else {
		// 16-bit mode
		memory->write(addr, regs.X & 0x00FF);
		memory->write(addr + 1, (regs.X >> 8) & 0x00FF);
	}

	return cycles;
}

uint8_t Cpu::DEY(uint8_t cycles) {
	regs.Y--;

	regs.P.Z = (regs.Y == 0);

	if (regs.P.X) { 
		// 8-bit mode
		regs.P.N = (regs.Y & 0x0080) != 0;
	}
	else { 
		// 16-bit mode
		regs.P.N = (regs.Y & 0x8000) != 0;
	}

	return cycles;
}

uint8_t Cpu::BCC(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (!regs.P.C) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::TYA(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.A = (regs.A & 0xFF00) | (regs.Y & 0x00FF);
		regs.P.N = regs.A & 0x0080;
		regs.P.Z = regs.A & 0x00FF == 0;
	}
	else {
		// 16-bit mode
		regs.A = regs.Y;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}
	return cycles;
}

uint8_t Cpu::TXS(uint8_t cycles) {
	if (E) {
		// Emulation mode: XH forced to $01, 8-bit transfer
		regs.SP = (0x0100) | (regs.X & 0x00FF);
	}
	else {
		if (regs.P.X) {
			// Native mode, 8-bit index: Transfer lower 8 bits of X to SP
			regs.SP = (regs.SP & 0xFF00) | (regs.X & 0x00FF);
		}
		else {
			// Native mode, 16-bit index: Transfer 16-bit X to SP
			regs.SP = regs.X;
		}
	}
	return cycles;
}

uint8_t Cpu::TXY(uint8_t cycles) {
	regs.Y = regs.X;

	return cycles;
}

uint8_t Cpu::LDY(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.Y = (regs.Y & 0xFF00) | val;

		regs.P.N = regs.Y & 0x0080;
		regs.P.Z = (regs.Y & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.Y = val;

		regs.P.N = regs.Y & 0x8000;
		regs.P.Z = regs.Y == 0;
	}
	return cycles;
}

uint8_t Cpu::LDA(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.A = (regs.A & 0xFF00) | val;

		regs.P.N = regs.A & 0x0080;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.A = val;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}
	return cycles;
}

uint8_t Cpu::LDX(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		regs.X = (regs.X & 0xFF00) | val;

		regs.P.N = regs.X & 0x0080;
		regs.P.Z = (regs.X & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		regs.X = val;

		regs.P.N = regs.X & 0x8000;
		regs.P.Z = regs.X == 0;
	}
	return cycles;
}

uint8_t Cpu::TAY(uint8_t cycles) {
	regs.Y = regs.A;

	regs.P.N = (regs.P.X) ? (regs.Y & 0x0080) : (regs.Y & 0x8000);
	regs.P.Z = (regs.Y & ((regs.P.X) ? 0x00FF : 0xFFFF)) == 0;

	return cycles;
}

uint8_t Cpu::TAX(uint8_t cycles) {
	regs.X = regs.A;

	regs.P.N = (regs.P.X) ? (regs.X & 0x0080) : (regs.X & 0x8000);
	regs.P.Z = (regs.X & ((regs.P.X) ? 0x00FF : 0xFFFF)) == 0;

	return cycles;
}

uint8_t Cpu::PLB(uint8_t cycles) {
	regs.DBR = pull();

	regs.P.N = regs.DBR & 0x80;
	regs.P.Z = regs.DBR == 0;

	return cycles;
}

uint8_t Cpu::BCS(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (regs.P.C) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::CLV(uint8_t cycles) {
	// Clear V flag
	regs.P.V = 0;

	return cycles;
}

uint8_t Cpu::TSX(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		regs.X = (regs.X & 0xFF00) | (regs.SP & 0x00FF);
		regs.P.N = regs.X & 0x0080;
		regs.P.Z = regs.X & 0x00FF == 0;
	}
	else {
		// 16-bit mode
		regs.X = regs.SP;

		regs.P.N = regs.X & 0x8000;
		regs.P.Z = regs.X == 0;
	}
	return cycles;
}

uint8_t Cpu::TYX(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		regs.X = (regs.X & 0xFF00) | (regs.Y & 0x00FF);
		regs.P.N = regs.X & 0x0080;
		regs.P.Z = regs.X & 0x00FF == 0;
	}
	else {
		// 16-bit mode
		regs.X = regs.Y;

		regs.P.N = regs.X & 0x8000;
		regs.P.Z = regs.X == 0;
	}
	return cycles;
}

uint8_t Cpu::CPY(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		uint8_t res = regs.Y - val;

		regs.P.N = res & 0x80;
		regs.P.Z = res == 0;
		regs.P.C = regs.Y >= val;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint16_t res = regs.Y - val;

		regs.P.N = res & 0x0080;
		regs.P.Z = res == 0;
		regs.P.C = regs.Y >= val;
	}
	return cycles;
}

uint8_t Cpu::CMP(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		uint8_t res = regs.A - val;

		regs.P.N = res & 0x80;
		regs.P.Z = res == 0;
		regs.P.C = regs.A >= val;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint16_t res = regs.A - val;

		regs.P.N = res & 0x0080;
		regs.P.Z = res == 0;
		regs.P.C = regs.A >= val;
	}
	return cycles;
}

uint8_t Cpu::CPX(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		uint8_t val = memory->read((this->*f)());
		uint8_t res = regs.X - val;

		regs.P.N = res & 0x80;
		regs.P.Z = res == 0;
		regs.P.C = regs.X >= val;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint16_t res = regs.X - val;

		regs.P.N = res & 0x0080;
		regs.P.Z = res == 0;
		regs.P.C = regs.X >= val;
	}
	return cycles;
}

uint8_t Cpu::SED(uint8_t cycles) {
	regs.P.D = 1;

	return cycles;
}

// TODO double check this
uint8_t Cpu::SBC(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read(addr);
		uint16_t res;
		if (regs.P.D) {
			// BCD mode
			res = (regs.A & 0x0F) - (val & 0x0F) - (1 - regs.P.C);
			if (res > 0x09) res -= 0x06;
			res += ((regs.A & 0xF0) - (val & 0xF0));
			if (res > 0x90) res -= 0x60;
		}
		else {
			// Binary mode
			res = regs.A - val - (1 - regs.P.C);
		}

		regs.P.V = ((regs.A ^ res) & 0x80) && ((regs.A ^ val) & 0x80);
		regs.P.C = res < 0x100;
		regs.P.N = res & 0x80;
		regs.P.Z = (uint8_t)res == 0;

		regs.A = (uint8_t)res;
	}
	else {
		// 16-bit mode
		uint16_t val = memory->read(addr) | (memory->read(addr + 1) << 8);
		uint32_t res;
		if (regs.P.D) {
			// BCD mode
			res = (regs.A & 0x0F) - (val & 0x0F) - (1 - regs.P.C);
			if (res > 0x09) res -= 0x06;
			res += ((regs.A & 0xF0) - (val & 0xF0));
			if (res > 0x90) res -= 0x60;
			res += ((regs.A & 0xFF00) - (val & 0xFF00));
			if (res > 0x9000) res -= 0x6000;
		}
		else {
			// Binary mode
			res = regs.A - val - (1 - regs.P.C);
		}

		regs.P.V = ((regs.A ^ res) & 0x8000) && ((regs.A ^ val) & 0x8000);
		regs.P.C = res < 0x10000;
		regs.P.N = res & 0x8000;
		regs.P.Z = (uint16_t)res == 0;

		regs.A = (uint16_t)res;
	}

	return cycles;
}

uint8_t Cpu::PLX(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		regs.X = (regs.X & 0xFF00) | pull();

		regs.P.N = regs.X & 0x0080;
		regs.P.Z = (regs.X & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		regs.X = pull() | (pull() << 8);

		regs.P.N = regs.X & 0x8000;
		regs.P.Z = regs.X == 0;
	}

	return cycles;
}

uint8_t Cpu::XCE(uint8_t cycles) {
	// swap C and E flags
	uint8_t tmp = regs.P.C;
	regs.P.C = E;
	E = tmp;

	return cycles;
}

uint8_t Cpu::PEA(uint32_t(Cpu::*f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	push(addr & 0x00FF);
	push((addr & 0xFF00 >> 8));

	return cycles;
}

uint8_t Cpu::BEQ(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (regs.P.Z) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::NOP(uint8_t cycles) {
	return cycles;
}

uint8_t Cpu::XBA(uint8_t cycles) {
	uint8_t hi = (regs.A & 0xFF00) >> 8;
	uint8_t lo = (regs.A & 0x00FF);

	regs.A = hi | lo << 8;

	regs.P.Z = (regs.A & 0x00FF) == 0;
	regs.P.N = regs.A & 0x0080;

	return cycles;
}

uint8_t Cpu::INX(uint8_t cycles) {
	regs.X++;

	regs.P.Z = (regs.X == 0);

	if (regs.P.X) {
		// 8-bit mode
		regs.P.N = (regs.X & 0x0080) != 0;
	}
	else {
		// 16-bit mode
		regs.P.N = (regs.X & 0x8000) != 0;
	}

	return cycles;
}

uint8_t Cpu::SEP(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint8_t val = memory->read((this->*f)());

	uint8_t N = (regs.P.N | ((val >> 7) & 1)) > 0;
	uint8_t V = (regs.P.V | ((val >> 6) & 1)) > 0;
	uint8_t M = (regs.P.M | ((val >> 5) & 1)) > 0;
	uint8_t X = (regs.P.X | ((val >> 4) & 1)) > 0;
	uint8_t boolB = (B | ((val >> 4) & 1)) > 0;
	uint8_t D = (regs.P.D | ((val >> 3) & 1)) > 0;
	uint8_t I = (regs.P.I | ((val >> 2) & 1)) > 0;
	uint8_t Z = (regs.P.Z | ((val >> 1) & 1)) > 0;
	uint8_t C = (regs.P.C | (val & 1)) > 0;

	regs.P.N = N;
	regs.P.V = V;

	if (!E) {
		regs.P.M = M;
		regs.P.X = X;
	}
	else {
		B = boolB;
		regs.P.M = true;
		regs.P.X = true;
	}

	regs.P.D = D;
	regs.P.I = I;
	regs.P.Z = Z;
	regs.P.C = C;

	return cycles;
}

uint8_t Cpu::PHX(uint8_t cycles) {
	if (regs.P.X) {
		// 8-bit mode
		push(regs.X & 0x00FF);
	}
	else {
		// 16-bit mode
		push(regs.X & 0x00FF);
		push(regs.X >> 8);
	}

	return cycles;
}

uint8_t Cpu::STP(uint8_t cycles) {
	stopped = true;

	return cycles;
}

uint8_t Cpu::CLD(uint8_t cycles) {
	regs.P.D = 0;

	return cycles;
}

uint8_t Cpu::REP(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint8_t val = memory->read((this->*f)());

	bool N = ((uint8_t)regs.P.N & ~((val >> 7) & 1)) > 0;
	bool V = ((uint8_t)regs.P.V & ~((val >> 6) & 1)) > 0;
	bool M = ((uint8_t)regs.P.M & ~((val >> 5) & 1)) > 0;
	bool X = ((uint8_t)regs.P.X & ~((val >> 4) & 1)) > 0;
	bool boolB = ((uint8_t)B & ~((val >> 4) & 1)) > 0;
	bool D = ((uint8_t)regs.P.D & ~((val >> 3) & 1)) > 0;
	bool I = ((uint8_t)regs.P.I & ~((val >> 2) & 1)) > 0;
	bool Z = ((uint8_t)regs.P.Z & ~((val >> 1) & 1)) > 0;
	bool C = ((uint8_t)regs.P.C & ~(val & 1)) > 0;

	regs.P.N = N;
	regs.P.V = V;
	
	if (!E) {
		regs.P.M = M;
		regs.P.X = X;
	}
	else {
		B = boolB;
		regs.P.M = true;
		regs.P.X = true;
	}

	regs.P.D = D;
	regs.P.I = I;
	regs.P.Z = Z;
	regs.P.C = C;

	return cycles;
}

// accumulator only
uint8_t Cpu::DEC(uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		regs.A--;

		regs.P.N = regs.A & 0x80;
		regs.P.Z = regs.A == 0;
	}
	else {
		// 16-bit mode
		regs.A--;

		regs.P.N = regs.A & 0x8000;
		regs.P.Z = regs.A == 0;
	}

	return cycles;
}

uint8_t Cpu::TSC(uint8_t cycles) {
	regs.A = regs.SP;

	regs.P.N = regs.A & 0x8000;
	regs.P.Z = regs.A == 0;

	return cycles;
}

uint8_t Cpu::DEC(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory->read(addr);
		val--;
		memory->write(addr, val);

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint16_t val = memory->read(addr) | memory->read(addr + 1) << 8;
		val--;
		memory->write(addr, val);

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
	}

	return cycles;
}

uint8_t Cpu::INY(uint8_t cycles) {
	regs.Y++;

	regs.P.Z = (regs.Y == 0);

	if (regs.P.X) {
		// 8-bit mode
		regs.P.N = (regs.Y & 0x0080) != 0;
	}
	else {
		// 16-bit mode
		regs.P.N = (regs.Y & 0x8000) != 0;
	}

	return cycles;
}

uint8_t Cpu::DEX(uint8_t cycles) {
	regs.X--;

	regs.P.Z = (regs.X == 0);

	if (regs.P.X) {
		// 8-bit mode
		regs.P.N = (regs.X & 0x0080) != 0;
	}
	else {
		// 16-bit mode
		regs.P.N = (regs.X & 0x8000) != 0;
	}

	return cycles;
}

uint8_t Cpu::WAI(uint8_t cycles) {
	wai = 1;

	return cycles;
}

uint8_t Cpu::BNE(uint8_t cycles) {
	int8_t offset = memory->read(regs.PC++);

	if (!regs.P.Z) {
		uint16_t oldPC = regs.PC;
		regs.PC += offset;

		// Check for page crossing
		if ((oldPC & 0xFF00) != (regs.PC & 0xFF00)) {
			cycles++;
		}
		cycles++;
	}

	return cycles;
}

uint8_t Cpu::PEI(uint8_t cycles) {
	uint16_t ptrLow = memory->read(regs.PC++);
	uint16_t ptrHigh = memory->read(regs.PC++);

	uint16_t effAddr = ptrLow | (ptrHigh << 8);

	push((effAddr >> 8) & 0xFF);
	push(effAddr & 0xFF);

	return cycles;
}

