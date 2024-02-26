#include "../include/ppu.hpp"
#include "../include/cpu.hpp"

void Ppu::step(int cycles) {
	scanline++;

	if (scanline == 225) {
		// enter vblank
		// set NMI
		cpu->setInterrupt(Interrupts::NMI);
		mode = PPU_MODE::VBLANK;
	}
	else if (scanline >= TOTAL_SCANLINES) {
		scanline -= TOTAL_SCANLINES;
		cpu->setInterrupt(Interrupts::NONE);
		mode = PPU_MODE::VISIBLE;
	}

	switch (mode) {
	case VBLANK:
		drawBackground2Bpp();
		break;
	case VISIBLE:
		break;
	}
}

void Ppu::writeVRAMHi(uint16_t address, uint8_t value) {
	vram[address & 0x7fff] = (vram[address & 0x7fff] & 0xff) | (value << 8);
}

void Ppu::writeVRAMLo(uint16_t address, uint8_t value) {
	vram[address & 0x7fff] = (vram[address & 0x7fff] & 0xff00) | value;
}

uint16_t Ppu::readVRAM(uint16_t address) {
	return vram[address & 0x7FFF];
}

uint16_t Ppu::readCGRAM(uint8_t address) {
	return cgram[address];
}

bool Ppu::writeCGRAM(uint16_t address, uint8_t value) {
	// if address is even, remember the current value
	if (!cgramFlipFlop) {
		cgramLsb = value;
		cgramFlipFlop = true;
		return false;
	}
	else {
		cgram[address] = (cgramLsb << 8) | value;
		cgramFlipFlop = false;
		return true;
	}
}

uint32_t Ppu::getRGBAFromCGRAM(uint8_t id) {
	uint8_t r = (cgram[id] & 0b11111) * 255 / 31;
	uint8_t g = ((cgram[id] >> 5) & 0b11111) * 255 / 31;
	uint8_t b = ((cgram[id] >> 10) & 0b11111) * 255 / 31;
	return (r << 24) | (g << 16) | (b << 8) | 0xff;
}

void Ppu::drawBackground2Bpp() {
	for (uint16_t a = 0x0000; a < 0x3a0; a++) {
		uint16_t tile_id = vram[0x7c00 + a];
		uint16_t tile_address = tile_id * 8;

		for (int i = 0; i < 8; i++) {
			const uint8_t b_lo = vram[tile_address + i] >> 8;
			const uint8_t b_hi = vram[tile_address + i] & 0xff;

			for (int j = 0; j < 8; j++) {
				uint8_t v = (((b_lo >> (7 - j)) & 1) + (2 * ((b_hi >> (7 - j)) & 1))) > 0;
				drawPixel(a % 32 * 8 + j, a / 32 * 8 + i, getRGBAFromCGRAM(v));
			}
		}
	}
}

void Ppu::writeBGBaseAddrScreenSize(uint8_t id, uint8_t value) {
	bgBase[id] = ((value >> 2) << 10) & 0x7FFF;

	switch (value & 0b11) {
	case 0b00: bgTilesH[id] = 32; bgTilesV[id] = 32; break;
	case 0b01: bgTilesH[id] = 64; bgTilesV[id] = 32; break;
	case 0b10: bgTilesH[id] = 32; bgTilesV[id] = 64; break;
	case 0b11: bgTilesH[id] = 64; bgTilesV[id] = 64; break;
	}
}

void Ppu::writeBGTileBase(uint8_t id, uint8_t value) {
	bgTileBase[id] = (value * 0x2000) % 0x10000 / 2;
}

void Ppu::reset() {
	scanline = 0;
	mode = PPU_MODE::VISIBLE;

	// zero out framebuffer
	for (int i = 0; i < XRES * YRES * COLOR_DEPTH; i++) {
		frameBuffer[i] = 0;
	}
}

void Ppu::drawPixel(int x, int y, uint32_t v) {
	frameBuffer[y * 4 * 256 + x * 4] = v >> 16;
	frameBuffer[y * 4 * 256 + x * 4 + 1] = v >> 8 & 0xff;
	frameBuffer[y * 4 * 256 + x * 4 + 2] = v & 0xff;
	frameBuffer[y * 4 * 256 + x * 4 + 3] = 0xff;
}

uint8_t* Ppu::getFrameBuffer() {
	return frameBuffer;
}