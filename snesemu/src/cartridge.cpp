#include "../include/cartridge.hpp"

void Cartridge::loadRom(std::string path) {
    std::cout << "lorom size is " << loRom.size();
    std::ifstream file;
    file.open(path, std::ios::ate);
    size_t fileSize = file.tellg();
    file.seekg(0);
    size_t totalBytesRead = 0;
    // LoROM header at $7FC0

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

    // return reset vector
    std::cout << (loRom[0x7FFC] | (loRom[0x7FFD] << 8));
}

uint8_t Cartridge::read(uint32_t address) {
    return loRom[address];
}

uint32_t Cartridge::getResetVector() {
    return loRom[0x7FFC] | (loRom[0x7FFD] << 8);
}