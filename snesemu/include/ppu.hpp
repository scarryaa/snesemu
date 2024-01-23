#ifndef PPU_HPP
#define PPU_HPP

#include <stdint.h>

enum PPU_MODE {
	VBLANK,
	VISIBLE
};

class Ppu {
public:
	Ppu() : scanline(0), mode(PPU_MODE::VISIBLE) {}
	void step(int cycles);
	void drawPixel(int x, int y, uint32_t color);
	uint8_t* getFrameBuffer();
	uint8_t vram[0x20000];

private:
	static const int XRES = 256;
	static const int YRES = 240;
	static const int COLOR_DEPTH = 4;
	static const int VBLANK_PERIOD = 70;
	static const int VISIBLE_SCANLINES = 224;

	PPU_MODE mode;
	int scanline;
	uint8_t frameBuffer[XRES * YRES * COLOR_DEPTH];

};

#endif