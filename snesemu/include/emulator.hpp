#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include "memory.hpp"

class Emulator
{
public:
	void loadRom(std::string path);

private:
	Memory memory;
};

#endif