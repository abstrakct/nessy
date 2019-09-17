#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "mappers/mapper000.h"

class Cartridge {
    public:
        Cartridge(const std::string& filename);
        ~Cartridge();

        bool isValid() { return valid; }

        enum Mirror {
            HORIZONTAL,
            VERTICAL,
            ONESCREEN_LO,
            ONESCREEN_HI
        } mirror = HORIZONTAL;

        bool cpuRead(uint16_t addr, uint8_t &data);
        bool cpuWrite(uint16_t addr, uint8_t data);

        bool ppuRead(uint16_t addr, uint8_t &data);
        bool ppuWrite(uint16_t addr, uint8_t data);

    private:
        std::vector<uint8_t> prgMem;
        std::vector<uint8_t> chrMem;
        int mapperNum, prgBanks, chrBanks;
        bool valid = false;

        std::shared_ptr<Mapper> mapper;
};
