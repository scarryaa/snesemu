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
	vram[address & 0x7fff] = (vram[address & 0x7fff] & 0xff00) | value;
}

void Ppu::writeVRAMLo(uint16_t address, uint8_t value) {
	vram[address & 0x7fff] = (vram[address & 0x7fff] & 0xff) | (value << 8);
}

uint16_t Ppu::readVRAM(uint16_t address) {
	return vram[address & 0x7FFF];
}

uint16_t Ppu::readCGRAM(uint16_t address) {
	return cgram[address];
}

void Ppu::writeCGRAM(uint16_t address, uint8_t value, std::function<void()> f) {
	// if address is even, remember the current value
	if (!cgramFlipFlop) {
		cgramLsb = value;
		cgramFlipFlop = true;
	}
	else {
		cgram[address] = (value << 8) | cgramLsb;
		cgramFlipFlop = false
		(f)();
	}
}

void Ppu::drawBackground2Bpp() {
	for (int pixel = 0; pixel < XRES * YRES; pixel++) {
		int x = pixel % XRES;
		int y = pixel / XRES;
		int i = x % 8;
		int j = y % 8;

		uint16_t tileId = bgBase[0] + x / 8 + y / 8 * 16;
		uint16_t tileDataBit1 = vram[tileId + i + j * 2];
		uint16_t tileDataBit2 = vram[tileId + i + j * 2 + 1];

		uint8_t color = (tileDataBit2 << 1) | tileDataBit1;
		uint16_t colorHigh = ((color & 0xE0) << 7) | ((color & 0x1C) << 3) | ((color & 0x03) << 2);

		drawPixel(x, y, colorHigh);
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

void Ppu::drawPixel(int x, int y, uint32_t color) {
	if (x < 0 || x >= XRES || y < 0 || y >= YRES)
	{
		return;
	}

	int index = (y * XRES + x) * 3;
	this->frameBuffer[index] = color & 0xFF;             // Blue
	this->frameBuffer[index + 1] = (color >> 8) & 0xFF;  // Green
	this->frameBuffer[index + 2] = (color >> 16) & 0xFF; // Red
}

uint8_t* Ppu::getFrameBuffer() {
	return frameBuffer;
}