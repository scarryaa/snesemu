#include <iostream>
#include "../include/emulator.hpp"
#include "../include/logger.hpp"

int main(int argv, char** args)
{
	Logger::getInstance()->openLog("log.txt");

	Emulator emulator;
	emulator.loadRom("roms/Super Mario World (U) [!].smc");
	/*emulator.set_PC_to_reset_vector();*/
	emulator.run();

	return 1;
}