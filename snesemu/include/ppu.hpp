#ifndef PPU_HPP
#define PPU_HPP

#include <stdint.h>
#include <functional>

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

	void writeVRAMHi(uint16_t address, uint8_t value);
	void writeVRAMLo(uint16_t address, uint8_t value);
	bool writeCGRAM(uint16_t address, uint8_t value);
	uint16_t readVRAM(uint16_t address);
	uint16_t readCGRAM(uint8_t address);
	void drawBackground2Bpp();
	void writeBGTileBase(uint8_t id, uint8_t value);
	void writeBGBaseAddrScreenSize(uint8_t id, uint8_t value);
	uint32_t getRGBAFromCGRAM(uint8_t id);

	void reset();
	void step(int cycles);
	void drawPixel(int x, int y, uint32_t color);
	uint8_t* getFrameBuffer();

private:
	static const int XRES = 256;
	static const int YRES = 239;
	static const int COLOR_DEPTH = 4;
	static const int VBLANK_PERIOD = 70;
	static const int TOTAL_SCANLINES = 262;
	int renderX;
	int renderY;

	enum PpuColorDepth {
		k2Bpp4Colors,
		k4Bpp16Colors,
		k8Bpp256Colors,
		kOffsetPerTile,
		kExtBg,
		kDisabled
	};

	PpuColorDepth PpuBgModes[8][4] = {
		{ k2Bpp4Colors, k2Bpp4Colors, k2Bpp4Colors, k2Bpp4Colors },		// mode 0 - 2bpp, 2bpp, 2bpp, 2bpp	Normal
		{ k4Bpp16Colors, k4Bpp16Colors, k2Bpp4Colors, kDisabled },		// mode 1 - 4bpp, 4bpp, 2bpp, -		Normal
		{ k4Bpp16Colors, k4Bpp16Colors, kOffsetPerTile, kDisabled },	// mode 2 - 4bpp, 4bpp, OPT, -		Offset-per-tile
		{ k8Bpp256Colors, k4Bpp16Colors, kDisabled, kDisabled },		// mode 3 - 8bpp, 4bpp -, -			Normal
		{ k8Bpp256Colors, k2Bpp4Colors, kOffsetPerTile, kDisabled },	// mode 4 - 8bpp, 2bpp, OPT, -		Offset-per-tile
		{ k4Bpp16Colors, k2Bpp4Colors, kDisabled, kDisabled },			// mode 5 - 4bpp, 2bpp - , -		512-pix-hires
		{ k4Bpp16Colors, kDisabled, kOffsetPerTile, kDisabled },		// mode 6 - 4bpp, -, OPT, -			512-pix plus OPT
		{ k8Bpp256Colors, kExtBg, kDisabled, kDisabled }				// mode 7 - 8bpp, EXTBG, -, -		Rotation/Scaling
	};

	bool cgramFlipFlop = false;
	uint8_t cgramLsb = 0;

	// vram, cgram
	uint16_t vram[0x8000];
	uint16_t cgram[0x100];

	// bg
	uint16_t bgTilesH[4] = { 32, 32, 32, 32 };
	uint16_t bgTilesV[4] = { 32, 32, 32, 32 };
	uint16_t bgBase[4] = { 0, 0, 0, 0 };
	uint16_t bgTileBase[4] = { 0, 0, 0, 0 };

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