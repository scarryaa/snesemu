#include "../include/memory.hpp"
#include "../include/logger.hpp"
#include "../include/cpu.hpp"
#include "../include/ppu.hpp"

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
            ppu->read((bank << 16) | offset);
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
            if (offset == 0x4210) {
                bool isNMI = cpu->isInterruptSet(Interrupts::NMI);
                cpu->setInterrupt(Interrupts::NONE);
                uint8_t val = (isNMI ? 0b10000000 : 0b00000000) | 0x02;
                return val;
            }

            // DMA, PPU2, hardware registers
            return dmaPPU2AndHardware[offset - 0x4200];
        } else if (offset >= 0x8000) {
            // LoROM section
            return cartridge->read((bank << 16) | offset);
        }
    }
    else if (bank <= 0x40 && bank >= 0x6F) {
        if (address >= 0x8000 && address <= 0xFFFF) {
            return cartridge->read((bank << 16) | offset);
        }
    }
    else if (bank >= 0x70 && bank <= 0x7D) {
        if (offset < 0x8000) {
            // Cartridge SRAM
            return sram[(bank - 0x70) * 0x8000 + offset];
        }
        else {
            // LoROM section
            return cartridge->read((bank * 0x8000) + (offset - 0x8000));
        }
    }
    else if (bank >= 0x7E && bank <= 0x7F) {
        return wram[(address - 0x7E * 0x10000 + offset)];
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        return read(((bank - 0x80) << 16) | offset);
    }
    else if (bank >= 0xC0 && bank <= 0xEF) {
        // Mirror of $40-$5F
        return read((bank - 0x80) << 16 | offset);
    }
    else if (bank >= 0xF0 && bank <= 0xFD) {
        // Mirror of $70-$7D
        return read((bank - 0x80) << 16 | offset);
    }
    else if (bank >= 0xFE && bank <= 0xFF) {
        if (offset > 0x8000 && offset <= 0xFFFF) {
            return cartridge->read((bank << 16) | offset);
        }
    }
    // TODO other banks and offsets

    // Default return
    std::ostringstream oss;
    oss << "Invalid address read: " << std::hex << std::uppercase << address;
    Logger::getInstance()->logError(oss.str());
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
            return;
        }
        else if (offset >= 0x2100 && offset <= 0x21FF) {
            // PPU1, APU, hardware registers
            ppu->write((bank << 16) | offset, value);
            return;
        }
        else if (offset >= 0x3000 && offset <= 0x3FFF) {
            // DSP, SuperFX, hardware registers
            dspSuperFXAndHardware[offset - 0x3000] = value;
            return;
        }
        else if (offset >= 0x4000 && offset <= 0x40FF) {
            // old style joypad registers
            oldJoypad[offset - 0x4000] = value;
            return;
        }
        else if (offset >= 0x4200 && offset <= 0x44FF)
        {
            // DMA, PPU2, hardware registers
            dmaPPU2AndHardware[offset - 0x4200] = value;
            return;
        }
        // TODO other regions
    }
    else if (bank >= 0x80 && bank <= 0xBF) {
        // Mirror of $00–$3F
        uint8_t addr = ((bank - 0x80) << 16) | offset;
        if (!(addr >= 0x00 && addr <= 0x3F)) return;
        write(((bank - 0x80) << 16) | offset, value);
        return;
    }
    else if (bank >= 0x70 && bank <= 0x7D) {
        if (offset < 0x8000) {
            // Cartridge SRAM
            sram[(bank - 0x70) * 0x8000 + offset] = value;
            return;
        }
    }
    // TODO other banks and offsets

    // Log invalid writes
    std::ostringstream oss;
    oss << "Invalid address write: " << std::hex << std::uppercase << address;
    Logger::getInstance()->logError(oss.str());
}