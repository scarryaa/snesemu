#include "../include/cpu.hpp"

uint32_t Cpu::getImm_8()
{
	regs.PC++;
	return (regs.PBR << 16) | regs.PC;
}

uint32_t Cpu::getImm_16()
{
	regs.PC += 2;
	return (regs.PBR << 16) | regs.PC - 1;
}

uint32_t Cpu::getAbs()
{
	regs.PC += 2;
	uint32_t addr = ((memory.read((regs.PBR << 16) | 
		regs.PC) << 8) | memory.read((regs.PBR << 16) | regs.PC - 1));
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsLong()
{
	regs.PC += 3;
	return ((memory.read((regs.PBR << 16) | regs.PC) << 16) | 
		(memory.read((regs.PBR << 16) | regs.PC - 1) << 8) | 
		memory.read((regs.PBR << 16) | regs.PC - 2));
}

uint32_t Cpu::getAbsIndexedX()
{
	regs.PC += 2;
	uint16_t addr = ((memory.read((regs.PBR << 16) | regs.PC) <<
		8) | memory.read((regs.PBR << 16) | regs.PC - 1));
	pbr = (addr & 0xFF00) != ((addr + regs.X) & 0xFF00);
	addr += regs.X;
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsIndexedY() {
	uint16_t lowByte = memory.read((regs.PBR << 16) | regs.PC);
	regs.PC++;
	uint16_t highByte = memory.read((regs.PBR << 16) | regs.PC);
	regs.PC++;

	uint16_t addr = (highByte << 8) | lowByte;
	pbr = (addr & 0xFF00) != ((addr + regs.Y) & 0xFF00);
	addr += regs.Y;

	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::getAbsLongIndexedX()
{
	regs.PC += 3;
	return ((memory.read((regs.PBR << 16) | regs.PC) <<
		16) | (memory.read((regs.PBR << 16) | regs.PC - 1) << 8) |
		memory.read((regs.PBR << 16) | regs.PC - 2)) + regs.X;
}

uint32_t Cpu::getAbsIndirect()
{
	regs.PC += 2;
	uint8_t lo = memory.read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory.read((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory.read(addr);
	uint8_t i_hi = memory.read(addr + 1);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::getAbsIndirectLong()
{
	regs.PC += 2;
	uint8_t lo = memory.read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory.read((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory.read(addr);
	uint8_t i_hi = memory.read(addr + 1);
	regs.PBR = memory.read(addr + 2);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::getAbsIndexedIndirectX()
{
	regs.PC += 2;
	uint8_t lo = memory.read((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = memory.read((regs.PBR << 16) | regs.PC);
	uint32_t addr = (hi << 8) | lo;
	uint8_t i_lo = memory.read(addr);
	uint8_t i_hi = memory.read(addr + 1);
	return (regs.PBR << 16) | (i_hi) << 8 | i_lo;
}

uint32_t Cpu::getLong()
{
	regs.PC += 3;
	return (memory.read((regs.PBR << 16) | regs.PC) << 16) |
		(memory.read((regs.PBR << 16) | regs.PC - 1) << 8) |
		memory.read((regs.PBR << 16) | regs.PC - 2);
}

uint32_t Cpu::getDirectPage()
{
	regs.PC += 1;
	return memory.read((regs.PBR << 16) | regs.PC) + regs.D;
}

uint32_t Cpu::getDirectPageIndexedX()
{
	regs.PC++;
	return memory.read((regs.PBR << 16) | regs.PC) + regs.X;
}

uint32_t Cpu::getDirectPageIndexedY()
{
	regs.PC++;
	return memory.read((regs.PBR << 16) | regs.PC) + regs.Y;
}

uint32_t Cpu::getDirectPageIndirect()
{
	regs.PC++;
	uint8_t dp_index = memory.read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory.read(dp_index + 2) << 16) 
		| (memory.read(dp_index));
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::getDirectPageIndirectLong()
{
	regs.PC++;
	uint8_t dp_index = memory.read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory.read(dp_index + 2) << 16) |
		(memory.read(dp_index + 1) << 8) | memory.read(dp_index);
	return dp_addr;
}

uint32_t Cpu::getDirectPageIndirectX()
{
	regs.PC++;
	uint8_t dp_index = memory.read(((regs.PBR << 16) | regs.PC) + regs.D) + regs.X;
	uint16_t dp_addr = (memory.read(dp_index + 1) << 8) |
		memory.read(dp_index);
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::getDirectPageIndirectIndexedY()
{
	regs.PC++;
	uint8_t dp_index = memory.read(((regs.PBR << 16) |
		regs.PC)) + regs.D;
	uint32_t dp_addr = (regs.DBR << 16) | (memory.read(dp_index + 1) << 8) | 
		memory.read(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::getDirectPageIndirectLongIndexedY()
{
	regs.PC++;
	uint8_t dp_index = memory.read(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (memory.read(dp_index + 2) << 16) |
		(memory.read(dp_index + 1) << 8) |
		memory.read(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::getStackRelative()
{
	regs.PC++;
	uint8_t byte = memory.read((regs.PBR << 16) | regs.PC);
	return regs.SP + byte;
}

uint32_t Cpu::getStackRelativeIndirectIndexedY()
{
	regs.PC++;
	uint8_t byte = memory.read((regs.PBR << 16) | regs.PC);
	uint8_t base = memory.read(((regs.DBR << 16) | regs.SP) + byte);
	return base + regs.Y;
}

uint8_t Cpu::step() {
	uint8_t opcode = memory.read(regs.PC++);
	uint8_t mFlagInv = !regs.P.M ? 1 : 0;
	uint8_t mFlagInv_2 = !regs.P.M ? 2 : 0;
	uint8_t dpL = (regs.D & 0x00FF) ? 1 : 0;

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
	case 0x20: return JSR(&Cpu::getAbs, 6);
	case 0x21: return AND(&Cpu::getDirectPageIndirectX, 6 + mFlagInv + dpL);
	case 0x22: return JSL(&Cpu::getAbsLong, 8);
	case 0x23: return AND(&Cpu::getStackRelative, 4 + mFlagInv);
	case 0x24: return BIT(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x25: return AND(&Cpu::getDirectPage, 3 + mFlagInv + dpL);
	case 0x26: return ROL(&Cpu::getDirectPage, 5 + mFlagInv + dpL);
	case 0x27: return AND(&Cpu::getDirectPageIndirectLong, 6 + mFlagInv + dpL);
	case 0x28: return PLP(4);
	case 0x29: return AND((regs.P.M) ? &Cpu::getImm_8 : &Cpu::getImm_16, 2);
	case 0x30: return BMI(2 + E ? 1 : 0);
	case 0x31: return AND(&Cpu::getDirectPageIndirectIndexedY, 5 + mFlagInv + dpL);
	case 0x32: return AND(&Cpu::getDirectPageIndirect, 5 + mFlagInv + dpL);
	case 0x33: return AND(&Cpu::getStackRelativeIndirectIndexedY, 7 + mFlagInv);
	case 0x34: return BIT(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x35: return AND(&Cpu::getDirectPageIndexedX, 4 + mFlagInv + dpL);
	case 0x36: return ROL(&Cpu::getDirectPageIndexedX, 6 + mFlagInv + dpL);
	case 0x37: return AND(&Cpu::getDirectPageIndirectLongIndexedY, 6 + mFlagInv + dpL);
	case 0x38: return SEC(2);
	}
}

uint8_t Cpu::BRK(uint32_t(Cpu::*f)(), uint8_t cycles) {
	// set BRK interrupt
	regs.P.I = 1;
	regs.P.D = 0;
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
		val = memory.read(addr);
		regs.P.Z = (regs.A & val & 0xFF) == 0;
		memory.write(addr, val | (regs.A & 0xFF));
	}
	else {
		// 16-bit mode
		val = memory.read(addr) | (memory.read(addr + 1) << 8);
		regs.P.Z = (regs.A & val) == 0;
		memory.write(addr, val | (regs.A & 0xFF));
		memory.write(addr + 1, (val >> 8) | (regs.A >> 8));
	}

	return cycles;
}


uint8_t Cpu::ORA(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory.read((this->*f)());
		regs.A = (regs.A & 0xFF00) | ((regs.A & 0x00FF) | val);

		regs.P.N = regs.A & 0x0080 ? 1 : 0;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
		regs.A |= val;

		regs.P.N = regs.A & 0x8000 ? 1 : 0;
		regs.P.Z = regs.A == 0;
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


// affects memory
uint8_t Cpu::ASL(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint32_t addr = (this->*f)();

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory.read(addr);
		regs.P.C = (val & 0x80) >> 7;
		val <<= 1;

		memory.write(addr, val);

		regs.P.N = (val & 0x80) ? 1 : 0;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
		regs.P.C = (val & 0x8000) >> 15;
		val <<= 1;

		memory.write(addr, val & 0xFF);
		memory.write(addr + 1, val >> 8);

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
	int8_t offset = memory.read(regs.PC++);

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
		uint8_t val = memory.read(addr);
		regs.P.Z = (regs.A & val & 0xFF) == 0;
		uint8_t result = val & ~(regs.A & 0xFF);
		memory.write(addr, result);
	}
	else {
		// 16-bit mode
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
		regs.P.Z = (regs.A & val) == 0;
		uint16_t result = val & ~regs.A;
		memory.write(addr, result & 0xFF);
		memory.write(addr + 1, result >> 8);
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
		uint8_t val = memory.read((this->*f)());
		regs.A = (regs.A & 0xFF00) | ((regs.A & 0x00FF) & val);

		regs.P.N = regs.A & 0x0080 ? 1 : 0;
		regs.P.Z = (regs.A & 0x00FF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
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

uint8_t Cpu::BIT(uint32_t(Cpu::* f)(), uint8_t cycles) {
	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory.read((this->*f)());
		uint8_t res = regs.A & val;

		regs.P.N = val & 0x80;
		regs.P.V = val & 0x40;
		regs.P.Z = res == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
		uint16_t res = regs.A & val;

		regs.P.N = val & 0x8000;
		regs.P.V = val & 0x4000;
		regs.P.Z = res == 0;
	}

	return cycles;
}

uint8_t Cpu::BIT_Imm(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint16_t val;

	if (regs.P.M) {
		// 8-bit mode
		val = memory.read((this->*f)());
		uint8_t res = regs.A & val;
		regs.P.Z = (res & 0xFF) == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		val = memory.read(addr) | (memory.read(addr + 1) << 8);
		uint16_t res = regs.A & val;
		regs.P.Z = res == 0;
	}

	return cycles;
}

uint8_t Cpu::ROL(uint32_t(Cpu::* f)(), uint8_t cycles) {
	uint8_t oldC = regs.P.C;

	if (regs.P.M) {
		// 8-bit mode
		uint8_t val = memory.read((this->*f)());
		regs.P.C = (val & 0x80) >> 7;
		val <<= 1;
		val |= oldC;

		regs.P.N = val & 0x80;
		regs.P.Z = val == 0;
	}
	else {
		// 16-bit mode
		uint32_t addr = (this->*f)();
		uint16_t val = memory.read(addr) | (memory.read(addr + 1) << 8);
		regs.P.C = (val & 0x8000) >> 15;
		val <<= 1;
		val |= oldC;

		regs.P.N = val & 0x8000;
		regs.P.Z = val == 0;
	}

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
	int8_t offset = memory.read(regs.PC++);

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