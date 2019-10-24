#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>

#include "memory.h"
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

        void reset();

    private:
        //std::vector<uint8_t> prgROM;
        //std::vector<uint8_t> chrROM;
        std::vector<uint8_t> prgRAM;
        //std::map<uint16_t, std::vector<uint8_t>> prgROMBanks;
        std::shared_ptr<BankedMemory> prgROM;
        std::shared_ptr<BankedMemory> chrROM;

        int mapperNum, prgBanks, chrBanks, prgRamSize;
        uint16_t firstBank, lastBank;
        bool valid = false;

        std::shared_ptr<Mapper> mapper;
        Cartridge::Mirror mirrorType = HORIZONTAL;
};
