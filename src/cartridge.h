#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "mapper.h"

class Cartridge {
    public:
        Cartridge(const std::string& filename);
        ~Cartridge();

        bool isValid() { return valid; }

        enum Mirror {
            ONESCREEN_LO = 0,
            ONESCREEN_HI,
            VERTICAL,
            HORIZONTAL,
            FOUR_SCREEN,
        };


        bool cpuRead(uint16_t addr, uint8_t &data);
        bool cpuWrite(uint16_t addr, uint8_t data);

        bool ppuRead(uint16_t addr, uint8_t &data);
        bool ppuWrite(uint16_t addr, uint8_t data);

        Cartridge::Mirror getMirrorType();

    private:
        std::vector<uint8_t> prgROM;
        std::vector<uint8_t> chrROM;
        std::vector<uint8_t> prgRAM;

        int mapperNum, prgBanks, chrBanks, prgRamSize;
        bool valid = false;

        std::shared_ptr<Mapper> mapper;
        Cartridge::Mirror mirrorType = HORIZONTAL;
};
