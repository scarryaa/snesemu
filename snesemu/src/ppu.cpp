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
		break;
	case VISIBLE:
		drawBackground();
		break;
	}
}

uint8_t Ppu::read(uint32_t address) {
	switch (address) {
		// Screen brightness and on/off
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

		// Sets mosiac bg effect
	case 0x2106:
		return bgMosiac;

		// 0x2107-0x210A Base Addr and Size of Bg Tilemaps
	case 0x2107:
		return bgTilemapAddrAndSize1;
	case 0x2108:
		return bgTilemapAddrAndSize2;
	case 0x2109:
		return bgTilemapAddrAndSize3;
	case 0x210A:
		return bgTilemapAddrAndSize4;

		// 0x210B-0x210C Background Character Addr registers
	case 0x210B:
		return bgCharacterAddr1;
	case 0x210C:
		return bgCharacterAddr2;

		// 0x210D-0x2114 Background scroll registers
	case 0x210D:
		return bgScroll1;
	case 0x210E:
		return bgScroll2;
	case 0x210F:
		return bgScroll3;
	case 0x2110:
		return bgScroll4;
	case 0x2111:
		return bgScroll5;
	case 0x2112:
		return bgScroll6;
	case 0x2113:
		return bgScroll7;
	case 0x2114:
		return bgScroll8;

		// 0x2115 VRAM address increment mode (1x or 32x?)
	case 0x2115:
		return vramAddrIncrementMode;

		// 0x2116-0x2117 VRAM address registers
	case 0x2116:
		return vramAddr1;
	case 0x2117:
		return vramAddr2;

		// 0x2133 SNES PPU mode select
	case 0x2133:
		return ppuMode;
	default:
		return 0xFF;
	}
}

void Ppu::write(uint32_t address, uint8_t value) {
	switch (address) {
		// Screen brightness and on/off
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

		// Sets mosiac bg effect
	case 0x2106:
		bgMosiac = value;
		break;

		// 0x2107-0x210A Base Addr and Size of Bg Tilemaps
	case 0x2107:
		bgTilemapAddrAndSize1 = value;
		break;
	case 0x2108:
		bgTilemapAddrAndSize2 = value;
		break;
	case 0x2109:
		bgTilemapAddrAndSize3 = value;
		break;
	case 0x210A:
		bgTilemapAddrAndSize4 = value;
		break;

		// 0x210B-0x210C Background Character Addr registers
	case 0x210B:
		bgCharacterAddr1 = value;
		break;
	case 0x210C:
		bgCharacterAddr2 = value;
		break;

		// 0x210D-0x2114 Background scroll registers
	case 0x210D:
		bgScroll1 = value;
		break;
	case 0x210E:
		bgScroll2 = value;
		break;
	case 0x210F:
		bgScroll3 = value;
		break;
	case 0x2110:
		bgScroll4 = value;
		break;
	case 0x2111:
		bgScroll5 = value;
		break;
	case 0x2112:
		bgScroll6 = value;
		break;
	case 0x2113:
		bgScroll7 = value;
		break;
	case 0x2114:
		bgScroll8 = value;
		break;

		// 0x2115 VRAM address increment mode (1x or 32x?)
	case 0x2115:
		vramAddrIncrementMode = value;
		break;

		// 0x2133 SNES PPU mode select
	case 0x2133:
		ppuMode = value;
		break;
	default:
		return;
	}
}

void Ppu::writeVRAMHi(uint16_t address, uint8_t value) {
	vram[address & 0x7FFF] = (vram[address & 0x7FFF] & 0xFF00) | value;
}

void Ppu::writeVRAMLo(uint16_t address, uint8_t value) {
	vram[address & 0x7FFF] = (vram[address & 0x7FFF] & 0xFF) | (value << 8);
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

void Ppu::drawBackground() {
	// Get location $210B and $210C
	uint32_t addr = bgCharacterAddr1;
	uint32_t addr2 = bgCharacterAddr2;
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