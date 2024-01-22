#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include "memory.hpp"
#include "cpu.hpp"

class Emulator
{
public:
	void loadRom(std::string path);

	Emulator() : cpu(&memory) {}

private:
	Memory memory;
	Cpu cpu;
};

#endif