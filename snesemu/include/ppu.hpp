#ifndef PPU_HPP
#define PPU_HPP

#include <stdint.h>

// TODO refactor this?
class Cpu;

enum PPU_MODE {
	VBLANK,
	VISIBLE
};

class Ppu {
public:
	Ppu() : scanline(0), mode(PPU_MODE::VISIBLE), renderX(0), renderY(0) {}

	void setCpu(Cpu& cpu) {
		this->cpu = &cpu;
	}

	uint8_t read(uint32_t address);
	void write(uint32_t address, uint8_t value);

	void drawBackground();

	void reset();
	void step(int cycles);
	void drawPixel(int x, int y, uint32_t color);
	uint8_t* getFrameBuffer();
	uint8_t vram[0x20000];

private:
	static const int XRES = 256;
	static const int YRES = 240;
	static const int COLOR_DEPTH = 4;
	static const int VBLANK_PERIOD = 70;
	static const int TOTAL_SCANLINES = 262;
	int renderX;
	int renderY;

	// registers
	uint8_t screenBrightnessAndOnOff;
	uint8_t spriteSizeAndBaseSpriteAddr;
	uint8_t vramAddrForSpriteData;
	uint8_t videoModeAndTileSize;
	uint8_t bgMosiac;
	uint8_t bgTilemapAddrAndSize1;
	uint8_t bgTilemapAddrAndSize2;
	uint8_t bgTilemapAddrAndSize3;
	uint8_t bgTilemapAddrAndSize4;
	uint8_t bgCharacterAddr1;
	uint8_t bgCharacterAddr2;
	uint8_t bgScroll1;
	uint8_t bgScroll2;
	uint8_t bgScroll3;
	uint8_t bgScroll4;
	uint8_t bgScroll5;
	uint8_t bgScroll6;
	uint8_t bgScroll7;
	uint8_t bgScroll8;
	uint8_t vramAddrIncrementMode;
	uint8_t vramAddr1;
	uint8_t vramAddr2;
	uint8_t ppuMode;

	PPU_MODE mode;
	int scanline;
	uint8_t frameBuffer[XRES * YRES * COLOR_DEPTH];
	Cpu* cpu;

};

#endif