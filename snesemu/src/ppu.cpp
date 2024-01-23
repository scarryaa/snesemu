#include "../include/ppu.hpp"

void Ppu::step(int cycles) {
	scanline++;

	if (scanline >= VISIBLE_SCANLINES) {
		// enter vblank
		scanline -= VISIBLE_SCANLINES;
		mode = PPU_MODE::VBLANK;
	}
	else {
		// enter regular mode
		mode = PPU_MODE::VISIBLE;
	}

	switch (mode) {
	case VBLANK:
	{
		for (int x = 0; x < XRES; x++) {
			for (int y = 0; y < YRES; y++) {
				drawPixel(x, y, 0xFFFFFF);
			}
		}
		break;
	}
	case VISIBLE:
		break;
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