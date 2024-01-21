#include "../include/cpu.hpp"

uint32_t Cpu::ADDR_getImm_8()
{
	regs.PC++;
	return (regs.PBR << 16) | regs.PC;
}

uint32_t Cpu::ADDR_getImm_16()
{
	regs.PC += 2;
	return (regs.PBR << 16) | regs.PC - 1;
}

uint32_t Cpu::ADDR_getAbs()
{
	regs.PC += 2;
	uint32_t addr = ((BUS_readFromMem((regs.PBR << 16) | 
		regs.PC) << 8) | BUS_readFromMem((regs.PBR << 16) | regs.PC - 1));
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::ADDR_getAbsLong()
{
	regs.PC += 3;
	return ((BUS_readFromMem((regs.PBR << 16) | regs.PC) << 16) | 
		(BUS_readFromMem((regs.PBR << 16) | regs.PC - 1) << 8) | 
		BUS_readFromMem((regs.PBR << 16) | regs.PC - 2));
}

uint32_t Cpu::ADDR_getAbsIndexedX()
{
	regs.PC += 2;
	uint16_t addr = ((BUS_readFromMem((regs.PBR << 16) | regs.PC) <<
		8) | BUS_readFromMem((regs.PBR << 16) | regs.PC - 1));
	pbr = (addr & 0xFF00) != ((addr + regs.X) & 0xFF00);
	addr += regs.X;
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::ADDR_getAbsIndexedY()
{
	regs.PC += 2;
	uint16_t addr = ((BUS_readFromMem((regs.PBR << 16) | regs.PC) <<
		8) | BUS_readFromMem((regs.PBR << 16) | regs.PC - 1));
	pbr = (addr & 0xFF00) != ((addr + regs.Y) * 0xFF00);
	addr += regs.Y;
	return (regs.DBR << 16) | addr;
}

uint32_t Cpu::ADDR_getAbsLongIndexedX()
{
	regs.PC += 3;
	return ((BUS_readFromMem((regs.PBR << 16) | regs.PC) <<
		16) | (BUS_readFromMem((regs.PBR << 16) | regs.PC - 1) << 8) |
		BUS_readFromMem((regs.PBR << 16) | regs.PC - 2)) + regs.X;
}

uint32_t Cpu::ADDR_getAbsIndirect()
{
	regs.PC += 2;
	uint8_t lo = BUS_readFromMem((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = BUS_readFromMem((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = BUS_readFromMem(addr);
	uint8_t i_hi = BUS_readFromMem(addr + 1);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::ADDR_getAbsIndirectLong()
{
	regs.PC += 2;
	uint8_t lo = BUS_readFromMem((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = BUS_readFromMem((regs.PBR << 16) | regs.PC);
	uint16_t addr = (hi << 8) | lo;
	uint8_t i_lo = BUS_readFromMem(addr);
	uint8_t i_hi = BUS_readFromMem(addr + 1);
	regs.PBR = BUS_readFromMem(addr + 2);
	return (regs.PBR << 16) | (i_hi << 8) | i_lo;
}

uint32_t Cpu::ADDR_getAbsIndexedIndirectX()
{
	regs.PC += 2;
	uint8_t lo = BUS_readFromMem((regs.PBR << 16) | regs.PC - 1);
	uint8_t hi = BUS_readFromMem((regs.PBR << 16) | regs.PC);
	uint32_t addr = (hi << 8) | lo;
	uint8_t i_lo = BUS_readFromMem(addr);
	uint8_t i_hi = BUS_readFromMem(addr + 1);
	return (regs.PBR << 16) | (i_hi) << 8 | i_lo;
}

uint32_t Cpu::ADDR_getLong()
{
	regs.PC += 3;
	return (BUS_readFromMem((regs.PBR << 16) | regs.PC) << 16) |
		(BUS_readFromMem((regs.PBR << 16) | regs.PC - 1) << 8) |
		BUS_readFromMem((regs.PBR << 16) | regs.PC - 2);
}

uint32_t Cpu::ADDR_getDirectPage()
{
	regs.PC += 1;
	return BUS_readfromMem((regs.PBR << 16) | regs.PC) + regs.D;
}

uint32_t Cpu::ADDR_getDirectPageIndexedX()
{
	regs.PC++;
	return BUS_readFromMem((regs.PBR << 16) | regs.PC) + regs.X;
}

uint32_t Cpu::ADDR_getDirectPageIndexedY()
{
	regs.PC++;
	return BUS_readFromMem((regs.PBR << 16) | regs.PC) + regs.Y;
}

uint32_t Cpu::ADDR_getDirectPageIndirect()
{
	regs.PC++;
	uint8_t dp_index = BUS_readFromMem(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (BUS_readFromMem(dp_index + 2) << 16) 
		| (BUS_readFromMem(dp_index));
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::ADDR_getDirectPageIndirectLong()
{
	regs.PC++;
	uint8_t dp_index = BUS_readFromMem(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (BUS_readFromMem(dp_index + 2) << 16) |
		(BUS_readFromMem(dp_index + 1) << 16) | BUS_readFromMem(dp_index);
	return dp_addr;
}

uint32_t Cpu::ADDR_getDirectPageIndirectX()
{
	regs.PC++;
	uint8_t dp_index = BUS_readFromMem(((regs.PBR << 16) | regs.PC) + regs.D) + regs.X;
	uint16_t dp_addr = (BUS_readFromMem(dp_index + 1) << 8) |
		BUS_readFromMem(dp_index);
	return (regs.DBR << 16) | dp_addr;
}

uint32_t Cpu::ADDR_getDirectPageIndirectIndexedY()
{
	regs.PC++;
	uint8_t dp_index = BUS_readFromMem(((regs.PBR << 16) |
		regs.PC)) + regs.D;
	uint32_t dp_addr = (regs.DBR << 16) | (BUS_readFromMem(dp_index + 1) << 8) | 
		bus_ReadFromMem(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::ADDR_getDirectPageIndirectLongIndexedY()
{
	regs.PC++;
	uint8_t dp_index = BUS_readFromMem(((regs.PBR << 16) | regs.PC)) + regs.D;
	uint32_t dp_addr = (BUS_readFromMem(dp_index + 2) << 16) |
		(BUS_readFromMem(dp_index + 1) << 8) |
		BUS_readFromMem(dp_index);
	pbr = (dp_addr & 0xFF00) != ((dp_addr + regs.Y) & 0xFF00);
	dp_addr += regs.Y;
	return dp_addr;
}

uint32_t Cpu::ADDR_getStackRelative()
{
	regs.PC++;
	uint8_t byte = BUS_readFromMem((regs.PBR << 16) | regs.PC);
	return regs.SP + byte;
}

uint32_t Cpu::ADDR_getStackRelativeIndirectIndexedY()
{
	regs.PC++;
	uint8_t byte = BUS_readFromMem((regs.PBR << 16) | regs.PC);
	uint8_t base = BUS_readFromMem(((regs.DBR << 16) | regs.SP) + byte);
	return base + regs.Y;
}
