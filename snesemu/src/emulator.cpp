#include "../include/emulator.hpp"

void Emulator::loadRom(std::string path)
{
	reset_vector = cartridge.getResetVector();
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

			cycles = cpu.step();
			cycles_to_run -= cycles;

			ppu.step(cycles);
		}

		window.postRender(ppu.getFrameBuffer());
	}
}

void Emulator::setPCToResetVector()
{
	cpu.setPC(reset_vector);
}