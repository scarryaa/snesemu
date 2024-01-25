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
	{
		break;
	}
	case VISIBLE:

		break;
	}
}

uint8_t Ppu::read(uint32_t address) {
	switch (address) {
		// 0x2100 - Screen brightness and on/off
	case 0x2100:
		return screenBrightnessAndOnOff;

		// Size of sprites and base addr for sprite tiles
	case 0x2101:
		return spriteSizeAndBaseSpriteAddr;

		// Set VRAM addr of sprite data
	case 0x2102:
		return vramAddrForSpriteData;

	case 0x2103:
		break;

		// Sets video mode and size of bg tiles
	case 0x2105:
		return videoModeAndTileSize;

	default:
		return 0xFF;
	}
}

void Ppu::write(uint32_t address, uint8_t value) {
	switch (address) {
		// 0x2100 - Screen brightness and on/off
	case 0x2100:
		screenBrightnessAndOnOff = value;
		break;

		// Size of sprites and base addr for sprite tiles
	case 0x2101:
		spriteSizeAndBaseSpriteAddr = value;
		break;

		// Set VRAM addr of sprite data
	case 0x2102:
		vramAddrForSpriteData = value;
		break;

	case 0x2103:
		break;

		// Sets video mode and size of bg tiles
	case 0x2105:
		videoModeAndTileSize = value;
		break;

	default:
		return;
	}
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