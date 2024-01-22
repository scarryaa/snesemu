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

uint32_t Cpu::getAbsIndexedY()
{
	regs.PC += 2;
	uint16_t addr = ((memory.read((regs.PBR << 16) | regs.PC) <<
		8) | memory.read((regs.PBR << 16) | regs.PC - 1));
	pbr = (addr & 0xFF00) != ((addr + regs.Y) * 0xFF00);
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

	// Notes
	// M,X = 1 8-bit (immediate only) +1 cycle
	// M,X = 0 16-bit (immediate only)
	switch (opcode)
	{
	case 0x00: return BRK(getImm_8, 0); // return 0 since we return 7 + !E in the interrupt method
	case 0x01: return ORA(getDirectPageIndirectX, 6 + !regs.P.M + (regs.D & 0x00FF) ? 1 : 0);
	case 0x02: return COP(getImm_8, 0); // return 0, same as above
	case 0x03: return ORA(getStackRelative, 4 + !regs.P.M);
	case 0x04: return TSB(getDirectPage, 5 + (!regs.P.M ? 2 : 0) + (regs.D & 0x00FF) ? 1 : 0);
	case 0x05: return ORA(getDirectPage, 3 + !regs.P.M + (regs.D & 0x00FF) ? 1 : 0);
	case 0x06: return ASL(getDirectPage, 5 + (!regs.P.M ? 2 : 0) + (regs.D & 0x00FF) ? 1 : 0);
	case 0x07: return ORA(getDirectPageIndirectLong, 6 + !regs.P.M + (regs.D & 0x00FF) ? 1 : 0);
	case 0x08: return PHP(3);
	case 0x09: return (regs.P.M) ? ORA(getImm_8, 2) : ORA(getImm_16, 3);
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
		regs.A |= val;

		regs.P.N = regs.A & 0x80 ? 1 : 0;
		regs.P.Z = regs.A == 0;
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
