#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <string>
#include <chrono>
#include <iomanip>
#include "memory.hpp"
#include "cpu.hpp"
#include "window.hpp"
#include "ppu.hpp"
#include "debug/disassembler.hpp"

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

	bool isBreakpoint() {
		return false;
	}

	void clearBreakpoint() {

	}

	void setBreakpoint() {

	}

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
};

#endif