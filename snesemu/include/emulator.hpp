#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include <chrono>
#include "memory.hpp"
#include "cpu.hpp"
#include "window.hpp"

class Emulator
{
public:
	void loadRom(std::string path);
	void run();

	Emulator() : cpu(&memory) {}

private:
	Memory memory;
	Cpu cpu;
	Window window;
	
	bool paused;
	bool quit;
};

#endif