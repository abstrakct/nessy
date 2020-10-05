#include <iostream>
#include "mapper004.h"

Mapper004::Mapper004(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);

    lastBank = p - 1;
}

Mapper004::~Mapper004()
{
}

std::vector<std::string> Mapper004::getInfoStrings()
{
    if (updateInfo)
    {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 004");
        //sprintf(line, "CHR BANK: %d", chrBank);
        //infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper004::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xA000, 1);
    prgROM->setBank(0xC000, lastBank - 1);
    prgROM->setBank(0xE000, lastBank);

    //chrROM->setBank(0x0000, 0);
    //chrROM->setBank(0x1000, 1);

    updateInfo = true;
}

void Mapper004::apply()
{
    //chrROM->setBank(0x0000, (chrBank * 2) + 0);
    //chrROM->setBank(0x1000, (chrBank * 2) + 1);

    updateInfo = true;
}

bool Mapper004::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000)
    {
        return true;
    }
    return false;
}

bool Mapper004::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper004::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr < 0xA000)
    {
        return true;
    }

    return false;
}

bool Mapper004::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper004::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr < 0x2000) {
    //    data = vram[addr];
    //    return true;
    //}
    return false;
}

bool Mapper004::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper004::ppuWriteData(uint16_t addr, uint8_t data)
{
    //if (addr < 0x2000) {
    //    vram[addr] = data;
    //    return true;
    //}
    return false;
}
