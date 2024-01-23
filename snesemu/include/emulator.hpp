#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include <chrono>
#include "memory.hpp"
#include "cpu.hpp"
#include "window.hpp"
#include "ppu.hpp"

class Emulator {
public:
	void loadRom(std::string path);
	void run();
	void setPCToResetVector();

	Emulator() : memory(&cartridge), cpu(&memory), paused(false), quit(false) {}

private:
	Memory memory;
	Cpu cpu;
	Window window;
	Ppu ppu;
	Cartridge cartridge;
	
	bool paused;
	bool quit;
	uint16_t reset_vector;
};

#endif