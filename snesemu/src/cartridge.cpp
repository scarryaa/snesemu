#include "../include/cartridge.hpp"

void Cartridge::loadRom(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open ROM file.");
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw std::runtime_error("Failed to read ROM file.");
    }

    uint16_t baseSNESHeader = 0xFFFF;
    bool isLoROM = true;
    bool fileHasHeader = (size & 0x3FF) == 0x2000;
    uint16_t fileSizeInKB = ((size - (fileHasHeader ? 0x200 : 0x000)) / 1024);

    if (isLoROM) {
        baseSNESHeader = 0x8000;

        uint8_t bank = 0x80;
        uint8_t shadowBank = 0x00;
        uint8_t chunks = fileSizeInKB / 32;
        
        for (uint32_t i = 0; i < size; i++) {
            for (uint8_t mirror = 0; mirror < (0x80 / chunks); mirror++) {
                if ((bank + (i / 0x8000) + (mirror * chunks)) < 0xFF) {
                    loRom[((bank + (i / 0x8000) + (mirror * chunks)) << 16) |
                        0x8000 + (i % 0x8000)] = buffer[i + (fileHasHeader ? 0x200 : 0x000)];
                }

                if ((shadowBank + (i / 0x8000) + (mirror * chunks)) < 0x7E) {
                    loRom[((shadowBank + (i / 0x8000) + (mirror * chunks)) << 16) | 0x8000 + (i % 0x8000)] = buffer[i + (fileHasHeader ? 0x200 : 0x000)];
                }
            }
        }
    }

    using namespace std;
    cout << "Check Mirroring:\n";
    cout << hex << loRom[0x808000] << " " << loRom[0x908000] << " " << loRom[0xa08000] << " " << loRom[0xb08000] << " \n";
    cout << hex << loRom[0x818000] << " " << loRom[0x948000] << " " << loRom[0xa58000] << " " << loRom[0xbb8000] << " \n";
    cout << hex << loRom[0xf18000] << " " << loRom[0xf48000] << " " << loRom[0xf58000] << " " << loRom[0xfb8000] << " \n";
    cout << hex << loRom[0x008000] << " " << loRom[0x208000] << " " << loRom[0x408000] << " " << loRom[0x508000] << " \n";

    uint8_t header[0x50];
    for (uint8_t i = 0; i < 0x50; i++) {
        header[i] = buffer[baseSNESHeader - 0x50 + i];
    }

    resetVector = (header[0x4d] << 8) | header[0x4c];
    std::cout << "loRom " << resetVector;
}

uint8_t Cartridge::read(uint32_t address) {
    return loRom[address];
}

uint32_t Cartridge::getResetVector() {
    return resetVector;
}