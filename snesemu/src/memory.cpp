#include "../include/memory.hpp"
#include "../include/logger.hpp"
#include "../include/generic_memory_command.hpp"

uint8_t Memory::read(uint32_t address) {
    // Mask the address to get bank and offset within the bank
    uint8_t bank = (address >> 16) & 0xFF;
    uint16_t offset = address & 0xFFFF;

    // Translate address based on bank and offset
    if (bank <= 0x3F) {
        if (offset < 0x2000) {
            // LowRAM, shadowed from bank $7E
            return wram[offset];
        }
        else if (offset >= 0x2100 && offset <= 0x21FF) {
            // PPU1, APU, hardware registers
            return apuAndHardware[offset - 0x2100];
        }
        else if (offset >= 0x8000) {
            // LoROM section
            return loRom[(bank * 0x8000) + (offset - 0x8000)];
        }
        // TODO other regions
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        return read((bank - 0x80) << 16 | offset);
    }
    else if (bank >= 0x70 && bank <= 0x7D) {
        if (offset < 0x8000) {
            // Cartridge SRAM
            return sram[(bank - 0x70) * 0x8000 + offset];
        }
        else {
            // LoROM section
            return loRom[((bank - 0x70) * 0x8000) + (offset - 0x8000)];
        }
    }
    // TODO other banks and offsets

    // Default return
    Logger::getInstance()->logError("Invalid address read: " + std::to_string(address));
    return 0xFF;
}

void Memory::write(uint32_t address, uint8_t value)
{
    // Mask the address to get bank and offset within the bank
    uint8_t bank = (address >> 16) & 0xFF;
    uint16_t offset = address & 0xFFFF;

    // Translate address based on bank and offset
    if (bank <= 0x3F) {
        if (offset < 0x2000) {
            // LowRAM, shadowed from bank $7E
            wram[offset] = value;
        }
        else if (offset >= 0x2100 && offset <= 0x21FF) {
            // PPU1, APU, hardware registers
            apuAndHardware[offset - 0x2100] = value;
        }
        else if (offset >= 0x8000) {
            // LoROM section
            loRom[(bank * 0x8000) + (offset - 0x8000)] = value;
        }
        // TODO other regions
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        write((bank - 0x80) << 16 | offset, value);
    }
    else if (bank >= 0x70 && bank <= 0x7D) {
        if (offset < 0x8000) {
            // Cartridge SRAM
            sram[(bank - 0x70) * 0x8000 + offset] = value;
        }
        else {
            // LoROM section
            loRom[((bank - 0x70) * 0x8000) + (offset - 0x8000)] = value;
        }
    }
    // TODO other banks and offsets

    // Log invalid writes
    Logger::getInstance()->logError("Invalid address write: " + std::to_string(address));
}

void Memory::loadRom(std::string path)
{
	
}