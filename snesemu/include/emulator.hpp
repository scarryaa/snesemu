#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include <chrono>
#include <iomanip>
#include <set>
#include "memory.hpp"
#include "cpu.hpp"
#include "window.hpp"
#include "ppu.hpp"
#include "debug/disassembler.hpp"
#include "debug/breakpoint.hpp"

class Emulator {
public:
	void loadRom(std::string path);
	void run();
	void setPCToResetVector();
	void logState();
	void openLogFile();
	void closeLogFile();

	Disassembler getDisassembler() {
		return disassembler;
	}

	Memory* getMemory() {
		return &memory;
	}

	Cpu* getCpu() {
		return &cpu;
	}

	bool isBreakpoint(breakpoint_type_t type, long value) {
		for (auto breakpoint : breakpoints)
		{
			if (breakpoint.type == type && breakpoint.address == value)
			{
				return true;
			}
		}
		return false;
	}

	std::set<Breakpoint> getBreakpoints() {
		return breakpoints;
	}

	void clearBreakpoint(breakpoint_type_t type, uint16_t value) {
		breakpoints.erase({ type, value });
	}

	void setBreakpoint(breakpoint_type_t type, uint16_t value) {
		breakpoints.insert({ type, value });
	}

	void checkForBreakpoints();

	void pause() {
		paused = true;
	}

	void unpause() {
		paused = false;
	}

	bool isPaused() {
		return paused;
	}

	void step() {
		uint8_t cycles = cpu.step();
		ppu.step(cycles);
	}

	void reset() {
		cpu.reset();
		cpu.setPC(resetVector);
		ppu.reset();
	}

	Emulator() : memory(&cartridge), cpu(&memory), disassembler(&cpu, &memory), paused(false), quit(false) {
		ppu.setCpu(cpu);
		memory.setCpu(cpu);
		memory.setPpu(ppu);
	}

private:
	std::ofstream logFile;

	Cartridge cartridge;
	Memory memory;
	Cpu cpu;
	Window window;
	Ppu ppu;
	Disassembler disassembler;
	
	bool paused;
	bool quit;
	uint16_t resetVector;
	std::set<Breakpoint> breakpoints;
};

#endif