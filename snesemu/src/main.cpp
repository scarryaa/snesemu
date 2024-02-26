#include <iostream>
#include "../include/emulator.hpp"
#include "../include/logger.hpp"

int main(int argv, char** args)
{
	Emulator emulator;
	emulator.openLogFile();
	emulator.loadRom("roms/8x8BG1Map2BPP32x328PAL.sfc");
	emulator.setPCToResetVector();
	emulator.run();

	emulator.closeLogFile();
	return 1;
}