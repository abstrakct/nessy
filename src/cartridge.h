#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "mapper.h"
#include "memory.h"

class Cartridge
{
public:
    Cartridge(const std::string &filename);
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

    void romOverwrite(uint16_t addr, uint8_t data);

    Cartridge::Mirror getMirrorType();

    void reset();

    std::shared_ptr<Mapper> mapper;
    std::vector<std::string> getMapperInfo();
    std::shared_ptr<BankedMemory> getPrgROM() { return prgROM; };
    std::shared_ptr<BankedMemory> getChrROM() { return chrROM; };
    // std::shared_ptr<Mapper> getMapper()
    // {
    //     return mapper;
    // };

private:
    std::shared_ptr<BankedMemory> prgROM;
    std::shared_ptr<BankedMemory> chrROM;
    std::vector<uint8_t> prgRAM;

    int mapperNum, prgBanks, chrBanks, prgRamSize;
    uint16_t firstBank, lastBank;
    bool valid = false;

    Cartridge::Mirror mirrorType = HORIZONTAL;
};
