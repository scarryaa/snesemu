#include "../include/memory.hpp"
#include "../include/logger.hpp"

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
            return ppuAPUAndHardware[offset - 0x2100];
        }
        else if (offset >= 0x3000 && offset <= 0x3FFF) {
            // DSP, SuperFX, hardware registers
            return dspSuperFXAndHardware[offset - 0x3000];
        }
        else if (offset >= 0x4000 && offset <= 0x40FF) {
            // old style joypad registers
            return oldJoypad[offset - 0x4000];
        } else if (offset >= 0x4200 && offset <= 0x44FF)
        {
            // DMA, PPU2, hardware registers
            return dmaPPU2AndHardware[offset - 0x4200];
        } else if (offset >= 0x8000) {
            // LoROM section
            return loRom[(bank * 0x8000) + (offset - 0x8000)];
        }
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        return read(((bank - 0x80) << 16) | offset);
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
            ppuAPUAndHardware[offset - 0x2100] = value;
        }
        // TODO other regions
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        uint8_t addr = ((bank - 0x80) << 16) | offset;
        if (!(addr >= 0x00 && addr <= 0x3F)) return;
        write(((bank - 0x80) << 16) | offset, value);
    }
    else if (bank >= 0x70 && bank <= 0x7D) {
        if (offset < 0x8000) {
            // Cartridge SRAM
            sram[(bank - 0x70) * 0x8000 + offset] = value;
        }
    }
    // TODO other banks and offsets

    // Log invalid writes
    Logger::getInstance()->logError("Invalid address write: " + std::to_string(address));
}

void Memory::loadRom(std::string path)
{
    std::ifstream file;
    file.open(path, std::ios::ate);
    size_t fileSize = file.tellg();
    file.seekg(0);
    size_t totalBytesRead = 0;

    const size_t BUFFER_SIZE = 4096;
    std::vector<char> buffer(BUFFER_SIZE);

    while (file.tellg() < fileSize) {
        file.read(&buffer[0], BUFFER_SIZE);
        size_t bytesRead = file.gcount();

        for (size_t i = 0; i < bytesRead; ++i) {
            size_t bank = totalBytesRead / 0x8000;
            size_t addr = (totalBytesRead % 0x8000) + 0x8000;
            size_t index = (bank * 0x4000) + (addr - 0x8000);

            if (index < ROM_SIZE) {
                loRom[index] = buffer[i];
            }
            else {
                throw std::runtime_error("Index exceeds ROM size.");
            }

            ++totalBytesRead;
        }
    }
}