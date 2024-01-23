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

    for (size_t i = 0; i < size; ++i) {
        loRom[i] = buffer[i];
    }

    std::cout << "loRom " << std::hex << (loRom[0x7FFC] | (loRom[0x7FFD] << 8));
}

uint8_t Cartridge::read(uint32_t address) {
    return loRom[address];
}

uint32_t Cartridge::getResetVector() {
    return loRom[0x7FFC] | (loRom[0x7FFD] << 8);
}