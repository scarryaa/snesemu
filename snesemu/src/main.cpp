#include <iostream>
#include "../include/emulator.hpp"
#include "../include/logger.hpp"

int main(int argv, char** args)
{
	Emulator emulator;
	emulator.openLogFile();
	emulator.loadRom("roms/CPUADC.sfc");
	emulator.setPCToResetVector();
	emulator.run();

	emulator.closeLogFile();
	return 1;
}