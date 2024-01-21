#include "../include/emulator.hpp"

void Emulator::loadRom(std::string path)
{
	memory.loadRom(path);
}