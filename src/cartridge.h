#pragma once

#include <cstdint>

class Cartridge {
    public:
        Cartridge();
        ~Cartridge();

        uint8_t cpuRead(uint16_t addr, bool readOnly = false);
        //uint8_t cpuRead(uint16_t addr, uint8_t &data); ???
        void cpuWrite(uint16_t addr, uint8_t data);

        uint8_t ppuRead(uint16_t addr, bool readOnly = false);
        void ppuWrite(uint16_t addr, uint8_t data);

    private:
};
