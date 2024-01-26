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
            uint8_t dmaId = ((offset & 0x00F0) >> 4);

            if (offset == 0x420B && value > 0) {
                startDMA(value);
            }
            else if (offset >= 0x4300 && offset <= 0x437A) {
                uint8_t lastDigit = offset & 0x000F;

                switch (lastDigit) {
                case 0x0: // DMA settings register
                    // 7 6 5 4 3 2 1 0
                    // 7 - transfer direction (0 = CPU/A to IO/B, 1 = IO/B to CPU/A)
                    // 6 - addressing mode (0 = direct table, 1 = indirect table) (HDMA only)
                    // 5 - Unused
                    // 4 - A Bus Address Step (0 = increment, 2 = decrement, 1/3 = None) (DMA Only)
                    // 3 2 1 0 - Unit Size & Transfer Format

                    // 000 transfer 1 byte    xx                    WRAM
                    // 001 transfer 2 bytes   xx, xx+1              VRAM
                    // 010 transfer 2 bytes   xx, xx                OAM or CGRAM
                    // 011 transfer 4 bytes   xx, xx, xx+1, xx+1    BGnxOFS
                    // 100 transfer 4 bytes   xx, xx+1, xx+2, xx+3  BGnSC
                    // 101 transfer 4 bytes   xx, xx_1, xx, xx+1    reserved
                    // 110 transfer 2 bytes   xx, xx                reserved
                    // 111 transfer 4 bytes   xx, xx, xx+1, xx+1    reserved
                    dma[dmaId].params = value;
                    break;
                case 0x1: // B address
                    dma[dmaId].bAddress = value;
                    break;
                case 0x2: // A Address Lo
                    dma[dmaId].aAddressLo = value;
                    break;
                case 0x3: // A Address Hi
                    dma[dmaId].aAddressHi = value;
                    break;
                case 0x4: // A Address Bank
                    dma[dmaId].aAddressBank = value;
                    break;
                case 0x5: // Number of Bytes To Transfer Lo
                    dma[dmaId].dmaNumberBytesToTransferLo = value;
                    break;
                case 0x6: // Number of Bytes To Transfer Hi
                    dma[dmaId].dmaNumberBytesToTransferHi = value;
                    break;
                case 0x7: // Data Bank (HDMA)
                    dma[dmaId].hdmaDataBank = value;
                    break;
                case 0x8: // A2 Table Address Lo
                    dma[dmaId].a2TableAddressLo = value;
                    break;
                case 0x9: // A2 Table Address Hi
                    dma[dmaId].a2TableAddressHi = value;
                    break;
                case 0xA: // Number of Lines to Transfer (HDMA)
                    dma[dmaId].hdmaNumberOfLinesToTransfer = value;
                    break;
                }
            }
            else {
                // TODO other HW registers
                // DMA, PPU2, hardware registers
                dmaPPU2AndHardware[offset - 0x4200] = value;
                return;
            }
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

void Memory::startDMA(uint8_t dmaId) {
    // dmaId indicates which DMA to start (DMA0 .. DMA7)

    uint16_t bytes = dma[dmaId].dmaNumberBytesToTransferHi << 8 | dma[dmaId].dmaNumberBytesToTransferLo;
    uint8_t mode = dma[dmaId].params & 0b111;
    uint8_t direction = dma[dmaId].params & 0b1;
    uint8_t bAddr = dma[dmaId].bAddress;
    uint32_t aAddr = (dma[dmaId].aAddressHi << 16) | (dma[dmaId].aAddressLo << 8);
    uint8_t dmaStep = dma[dmaId].params & 0b0001;

    bAddr += 0x2100;

    // while there are still bytes to transfer, continue
    while (bytes) {
        // switch based on how many bytes we want to transfer
        switch (mode) {
        case 0: // transfer 1 byte
            if (direction) {
                write(read(bAddr), aAddr);
            }
            else {
                write(read(aAddr), bAddr);
            }

            bytes--;
            aAddr += (dmaStep == 0) ? 1 : ((dmaStep == 2) ? -1 : 0);
            break;
        case 1: // transfer 2 bytes (xx, xx + 1)
            if (direction) {
                write(read(bAddr), aAddr);
                bytes--;
                write(read(bAddr + 1), aAddr + 1);
            }
            else {
                write(read(aAddr), bAddr);
                bytes--;
                write(read(aAddr + 1), bAddr + 1);
            }
            
            bytes--;
            aAddr += (dmaStep == 0) ? 2 : ((dmaStep == 2) ? -2 : 0);
            break;
        case 2: // transfer 2 bytes (xx, xx)
            if (direction) {
                write(read(bAddr), aAddr);
                bytes--;
                write(read(bAddr + 1), aAddr);
            }
            else {
                write(read(aAddr), bAddr);
                bytes--;
                write(read(aAddr + 1), bAddr);
            }

            bytes--;
            aAddr += (dmaStep == 0) ? 2 : ((dmaStep == 2) ? -2 : 0);
            break;
        case 3: // transfer 4 bytes (xx, xx, xx + 1, xx + 1)
            if (direction) {
                write(read(bAddr), aAddr);
                bytes--;
                write(read(bAddr + 1), aAddr);
                bytes--;
                write(read(bAddr + 2), aAddr + 1);
                bytes--;
                write(read(bAddr + 3), aAddr + 1);
            }
            else {
                write(read(aAddr), bAddr);
                bytes--;
                write(read(aAddr + 1), bAddr);
                bytes--;
                write(read(aAddr + 2), bAddr + 1);
                bytes--;
                write(read(aAddr + 3), bAddr + 1);
            }

            bytes--;
            aAddr += (dmaStep == 0) ? 4 : ((dmaStep == 2) ? -4 : 0);
            break;
        case 4: // trqansfer 4 bytes (xx, xx + 1, xx + 2, xx + 3)
            if (direction) {
                write(read(bAddr), aAddr);
                bytes--;
                write(read(bAddr + 1), aAddr + 1);
                bytes--;
                write(read(bAddr + 2), aAddr + 2);
                bytes--;
                write(read(bAddr + 3), aAddr + 3);
            }
            else {
                write(read(aAddr), bAddr);
                bytes--;
                write(read(aAddr + 1), bAddr + 1);
                bytes--;
                write(read(aAddr + 2), bAddr + 2);
                bytes--;
                write(read(aAddr + 3), bAddr + 3);
            }

            bytes--;
            aAddr += (dmaStep == 0) ? 4 : ((dmaStep == 2) ? -4 : 0);
            break;
        case 5:
        case 6:
        case 7:
        default:
            break;
        }
    }

    // reset DMA register
    write(0x420B, 0);
}