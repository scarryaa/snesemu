#include <iostream>
#include "../include/emulator.hpp"
#include "../include/logger.hpp"

int main(void)
{
	Logger::getInstance()->openLog("log.txt");

	Emulator emulator;
	emulator.loadRom("roms/Super Mario World (U) [!].smc");
}