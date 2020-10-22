#include "mapper003.h"
#include <iostream>

Mapper003::Mapper003(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);
}

Mapper003::~Mapper003()
{
}

std::vector<std::string> Mapper003::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 003");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);
        infoString.push_back("CHR Bank offset: 0x1000");
        infoString.push_back("CHR Bank size:   0x1000");
        sprintf(line, "CHR Bank:    %d", chrBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper003::reset()
{
    prgROM->setMappable(2);
    prgROM->setBank(0x8000, 0, 0);
    prgROM->setBank(0xC000, 1, 1);

    chrROM->setMappable(2);
    chrROM->setBank(0x0000, 0, 0);
    chrROM->setBank(0x1000, 1, 1);

    updateInfo = true;
}

void Mapper003::apply()
{
    chrROM->setBank(0x0000, (chrBank * 2) + 0, 0);
    chrROM->setBank(0x1000, (chrBank * 2) + 1, 1);

    updateInfo = true;
}

bool Mapper003::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }
    return false;
}

bool Mapper003::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper003::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        if (chrBanks > 4) {
            chrBank = data;
        } else {
            chrBank = (data & 0x3);
        }

        apply();
        //printf("[Mapper003] chrBank set to %d (%02X)\n", chrBank, chrBank);

        return true;
    }

    return false;
}

bool Mapper003::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper003::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr < 0x2000) {
    //    data = vram[addr];
    //    return true;
    //}
    return false;
}

bool Mapper003::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper003::ppuWriteData(uint16_t addr, uint8_t data)
{
    //if (addr < 0x2000) {
    //    vram[addr] = data;
    //    return true;
    //}
    return false;
}
