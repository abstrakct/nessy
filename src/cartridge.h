#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "mappers/mapper_000.h"

class Cartridge {
    public:
        Cartridge(const std::string& filename);
        ~Cartridge();

        //uint8_t cpuRead(uint16_t addr, bool readOnly = false);
        bool cpuRead(uint16_t addr, uint8_t &data);
        bool cpuWrite(uint16_t addr, uint8_t data);

        bool ppuRead(uint16_t addr, uint8_t &data);
        bool ppuWrite(uint16_t addr, uint8_t data);

    private:
        std::vector<uint8_t> prgMem;
        std::vector<uint8_t> chrMem;
        int mapperNum, prgBanks, chrBanks;

        std::shared_ptr<Mapper> mapper;
};
