#include "../include/emulator.hpp"

void Emulator::loadRom(std::string path)
{
	cartridge.loadRom(path);
	resetVector = cartridge.getResetVector();
}

void Emulator::run() {
	auto start_time = std::chrono::high_resolution_clock::now();
	int cycles_to_run = 0;

	while (!quit) {
		auto current_time = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
		start_time = current_time;
		const float max_elapsed = 1000.0f / 30.0f;

		if (elapsed > max_elapsed)
		{
			elapsed = max_elapsed;
		}
		cycles_to_run = (elapsed * Cpu::CLOCK_SPEED / 1000);
		quit = window.pollEvents();

		uint8_t cycles = 0;
		while (cycles_to_run > 0)
		{
			if (paused)
			{
				break;
			}

			logState();
			cycles = cpu.step();
			cycles_to_run -= cycles;

			ppu.step(cycles);
		}

		window.postRender(ppu.getFrameBuffer());
	}
}

void Emulator::openLogFile() {
	logFile.open("cpu.log", std::ios_base::app);
}

void Emulator::closeLogFile() {
	logFile.close();
}

void Emulator::setPCToResetVector()
{
	cpu.setPC(resetVector);
}

void Emulator::logState() {
	char buffer[256];

	snprintf(buffer, sizeof(buffer),
		"%06x OP:%02x A:%04x X:%04x Y:%04x S:%04x D:%04x DB:%02x %s%s%s%s%s%s%s%s",
		cpu.getPC(), cpu.getOpcode(), cpu.getA(), cpu.getX(), cpu.getY(),
		cpu.getSP(), cpu.getD(), cpu.getDBR(),
		cpu.getFlagN() ? "N" : "n",
		cpu.getFlagV() ? "V" : "v",
		cpu.getFlagE() ?
			"1" :
			cpu.getFlagM() ? "M" : "m",
		cpu.getFlagE() ? 
			cpu.getFlagB() ? "B" : "b" :
			cpu.getFlagX() ? "X" : "x",
		cpu.getFlagD() ? "D" : "d",
		cpu.getFlagI() ? "I" : "i",
		cpu.getFlagZ() ? "Z" : "z",
		cpu.getFlagC() ? "C" : "c");

	logFile << buffer << std::endl;
	logFile.flush();
}