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
            return memory[offset];
        }
        else if (offset >= 0x2100 && offset <= 0x21FF) {
            if (offset == 0x2139 || offset == 0x213a) {
                // PPU VMDataLo or PPU VMDataHi
                uint16_t adr = (memory[0x2117] << 8) | memory[0x2116];
                uint8_t _v_hi_lo = memory[0x2115] >> 7;
                uint8_t _v_trans = (memory[0x2115] & 0b1100) >> 2;
                uint8_t _v_step = memory[0x2115] & 0b11;
                uint16_t _t_st, _t_off, _t_in;
                switch (_v_trans) { //	PPU - Apply address translation if necessary (leftshift thrice lower 8, 9 or 10 bits)
                case 0b00:
                    break;
                case 0b01:		//	8 bit, aaaaaaaYYYxxxxx becomes aaaaaaaxxxxxYYY
                    _t_st = (adr & 0b1111111100000000);
                    _t_off = (adr & 0b11100000) >> 5;
                    _t_in = (adr & 0b11111) << 3;
                    adr = _t_st | _t_off | _t_in;
                    break;
                case 0b10:		//	9 bit, aaaaaaYYYxxxxxP becomes aaaaaaxxxxxPYYY
                    _t_st = (adr & 0b1111111000000000);
                    _t_off = (adr & 0b111000000) >> 6;
                    _t_in = (adr & 0b111111) << 3;
                    adr = _t_st | _t_off | _t_in;
                    break;
                case 0b11:		//	10 bit, aaaaaYYYxxxxxPP becomes aaaaaxxxxxPPYYY
                    _t_st = (adr & 0b1111110000000000);
                    _t_off = (adr & 0b1110000000) >> 7;
                    _t_in = (adr & 0b1111111) << 3;
                    adr = _t_st | _t_off | _t_in;
                    break;
                }
                if (((adr == 0x2139 && !_v_hi_lo) || (adr == 0x213a && _v_hi_lo)) && _v_trans != 0) {
                    uint16_t _t = (memory[0x2117] << 8) | memory[0x2116];
                    switch (_v_step)
                    {
                    case 0b00: _t += 1;	break;
                    case 0b01: _t += 32; break;
                    case 0b10: _t += 128; break;
                    case 0b11: _t += 128; break;
                    default: break;
                    }
                    memory[0x2116] = _t & 0xff;
                    memory[0x2117] = _t >> 8;
                }

                return (address == 0x2139) ? ppu->readVRAM(address) & 0xFF : ppu->readVRAM(address) >> 8;
            }
            else if (offset == 0x213b) { // PPU CGDATA - Palette Data Read
                auto res = ppu->readCGRAM(memory[0x2121]);
                memory[0x2121]++;
                return res;
            }
            else {
                return memory[bank << 16 | offset];
            }
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
            return memory[(bank - 0x70) * 0x8000 + offset];
        }
        else {
            // LoROM section
            return cartridge->read((bank * 0x8000) + (offset - 0x8000));
        }
    }
    else if (bank >= 0x7E && bank <= 0x7F) {
        return memory[(address - 0x7E * 0x10000 + offset)];
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
            memory[offset] = value;
            return;
        }
        else if (offset >= 0x2100 && offset <= 0x21FF) {
            // PPU1, APU, hardware registers
            if (address == 0x2107) {
                ppu->writeBGBaseAddrScreenSize(0, value);
            }
            else if (address == 0x2108) {
                ppu->writeBGBaseAddrScreenSize(1, value);
            }
            else if (address == 0x2109) {
                ppu->writeBGBaseAddrScreenSize(2, value);
            }
            else if (address == 0x210A) {
                ppu->writeBGBaseAddrScreenSize(3, value);
            } else if (address == 0x2118 || address == 0x2119) {
                //https://github.com/LilaQ/q00.snes/blob/master/bus.cpp
                uint16_t _adr = (memory[0x2117] << 8) | memory[0x2116];
                uint8_t _v_hi_lo = memory[0x2115] >> 7;
                uint8_t _v_trans = (memory[0x2115] & 0b1100) >> 2;
                uint8_t _v_step = memory[0x2115] & 0b11;
                switch (_v_trans) {		//	PPU - Apply address translation if necessary (leftshift thrice lower 8, 9 or 10 bits)
                case 0b00:
                    break;
                case 0b01: {		//	8 bit, aaaaaaaYYYxxxxx becomes aaaaaaaxxxxxYYY
                    uint16_t _t_st = (_adr & 0b1111111100000000);
                    uint16_t _t_off = (_adr & 0b11100000) >> 5;
                    uint16_t _t_in = (_adr & 0b11111) << 3;
                    _adr = _t_st | _t_off | _t_in;
                    break;
                }
                case 0b10: {		//	9 bit, aaaaaaYYYxxxxxP becomes aaaaaaxxxxxPYYY
                    uint16_t _t_st = (_adr & 0b1111111000000000);
                    uint16_t _t_off = (_adr & 0b111000000) >> 6;
                    uint16_t _t_in = (_adr & 0b111111) << 3;
                    _adr = _t_st | _t_off | _t_in;
                    break;
                }
                case 0b11: {		//	10 bit, aaaaaYYYxxxxxPP becomes aaaaaxxxxxPPYYY
                    uint16_t _t_st = (_adr & 0b1111110000000000);
                    uint16_t _t_off = (_adr & 0b1110000000) >> 7;
                    uint16_t _t_in = (_adr & 0b1111111) << 3;
                    _adr = _t_st | _t_off | _t_in;
                    break;
                }
                }
                if ((address == 0x2118 && !_v_hi_lo) || (address == 0x2119 && _v_hi_lo)) {
                    uint16_t _t = _adr;
                    switch (_v_step)
                    {
                    case 0b00: _t += 1;	break;
                    case 0b01: _t += 32; break;
                    case 0b10: _t += 128; break;
                    case 0b11: _t += 128; break;
                    default: break;
                    }
                    memory[0x2116] = _t & 0xff;
                    memory[0x2117] = _t >> 8;
                }
                if (address == 0x2118) {
                    ppu->writeVRAMLo(_adr, value);
                }
                else {
                    ppu->writeVRAMHi(_adr, value);
                }
            }
            else if (address == 0x2122) {
                ppu->writeCGRAM(memory[0x2121], value, [this]() { ppuCGRAMCallback(); });
            }
            else if (address == 0x210B) {
                // PPU BG/BG2 Tile Base
                ppu->writeBGTileBase(0, (value & 0xF));
                ppu->writeBGTileBase(1, (value >> 4));
            }
            else if (address == 0x210C) {
                // PPU BG3/BG4 Tile Base
                ppu->writeBGTileBase(2, (value & 0xF));
                ppu->writeBGTileBase(3, (value >> 4));
            }
            memory[offset] = value;
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
            if (offset == 0x420b && value > 0) {
                startDMA();
                return;
            }
            else memory[offset] = value;
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
            memory[(bank - 0x70) * 0x8000 + offset] = value;
            return;
        }
    }
    else {
        memory[bank * 0x8000 + offset] = value;
    }

    // Log invalid writes
    std::ostringstream oss;
    oss << "Invalid address write: " << std::hex << std::uppercase << address;
    Logger::getInstance()->logError(oss.str());
}

void Memory::startDMA() {
    // dmaId indicates which DMA to start (DMA0 .. DMA7)
    for (uint8_t dma_id = 0; dma_id < 8; dma_id++) {
        if ((memory[0x420b] & (1 << dma_id)) == (1 << dma_id)) {
            uint8_t io_address = memory[0x4301 + (dma_id * 0x10)];
            uint8_t val = memory[0x4300 + (dma_id * 0x10)];
            uint8_t dma_dir = val >> 7;					//	0 - write to IO, 1 - read from IO
            uint8_t dma_step = (val >> 3) & 0b11;		//	0 - increment, 2 - decrement, 1/3 = none
            uint8_t dma_mode = (val & 0b111);
            uint32_t cpu_address = (memory[0x4304 + (dma_id * 0x10)] << 16) | (memory[0x4303 + (dma_id * 0x10)] << 8) | memory[0x4302 + (dma_id * 0x10)];
            uint16_t bytes = (memory[0x4306 + (dma_id * 0x10)] << 8) | memory[0x4305 + (dma_id * 0x10)];
            while (bytes = BUS_DMAtransfer(dma_id, dma_mode, dma_dir, dma_step, cpu_address, io_address, bytes)) {}
        }
    }
    //	reset DMA-start register
    memory[0x420b] = 0x00;
}

uint16_t Memory::BUS_DMAtransfer(uint8_t dma_id, uint8_t dma_mode, uint8_t dma_dir, uint8_t dma_step, uint32_t& cpu_address, uint8_t io_address, uint16_t bytes_left) {
    switch (dma_mode) {
    case 0: {						//	transfer 1 byte (e.g. WRAM)
        if (!dma_dir)
            write(memory[cpu_address], 0x2100 + io_address);
        else
            write(read(0x2100 + io_address), cpu_address);
        if (!--bytes_left) return 0;
        cpu_address += (dma_step == 0) ? 1 : ((dma_step == 2) ? -1 : 0);
        break;
    }
    case 1:							//	transfer 2 bytes (xx, xx + 1) (e.g. VRAM)
        if (!dma_dir) {
            write(memory[cpu_address], 0x2100 + io_address);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 1], 0x2100 + io_address + 1);
            if (!--bytes_left) return 0;
        }
        else {
            write(read(0x2100 + io_address), cpu_address);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 1), cpu_address + 1);
            if (!--bytes_left) return 0;
        }
        cpu_address += (dma_step == 0) ? 2 : ((dma_step == 2) ? -2 : 0);
        break;
    case 2:							//	transfer 2 bytes (xx, xx) (e.g. OAM / CGRAM)
        if (!dma_dir) {
            write(memory[cpu_address], 0x2100 + io_address);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 1], 0x2100 + io_address);
            if (!--bytes_left) return 0;
        }
        else {
            write(read(0x2100 + io_address), cpu_address);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 1), cpu_address);
            if (!--bytes_left) return 0;
        }
        cpu_address += (dma_step == 0) ? 2 : ((dma_step == 2) ? -2 : 0);
        break;
    case 3:							//	transfer 4 bytes (xx, xx, xx + 1, xx + 1) (e.g. BGnxOFX, M7x)
        if (!dma_dir) {
            write(memory[cpu_address], 0x2100 + io_address);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 1], 0x2100 + io_address);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 2], 0x2100 + io_address + 1);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 3], 0x2100 + io_address + 1);
            if (!--bytes_left) return 0;
        }
        else {
            write(read(0x2100 + io_address), cpu_address);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 1), cpu_address);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 2), cpu_address + 1);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 3), cpu_address + 1);
            if (!--bytes_left) return 0;
        }
        cpu_address += (dma_step == 0) ? 4 : ((dma_step == 2) ? -4 : 0);
        break;
    case 4:							//	transfer 4 bytes (xx, xx + 1, xx + 2, xx + 3) (e.g. BGnSC, Window, APU...)
        if (!dma_dir) {
            write(memory[cpu_address], 0x2100 + io_address);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 1], 0x2100 + io_address + 1);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 2], 0x2100 + io_address + 2);
            if (!--bytes_left) return 0;
            write(memory[cpu_address + 3], 0x2100 + io_address + 3);
            if (!--bytes_left) return 0;
        }
        else {
            write(read(0x2100 + io_address), cpu_address);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 1), cpu_address + 1);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 2), cpu_address + 2);
            if (!--bytes_left) return 0;
            write(read(0x2100 + io_address + 3), cpu_address + 2);
            if (!--bytes_left) return 0;
        }
        cpu_address += (dma_step == 0) ? 4 : ((dma_step == 2) ? -4 : 0);
        break;
    case 5:					//	transfer 4 bytes (xx, xx + 1, xx, xx + 1) - RESERVED
        break;
    case 6:					//	same as mode 2 - RESERVED
        break;
    case 7:					//	same as mode 3 - RESERVED
        break;
    default:
        break;
    }
    return bytes_left;
}