#include <iostream>
#include "mapper033.h"
#include "../cartridge.h"

Mapper033::Mapper033(uint16_t p, uint16_t c) : Mapper(p, c)
{
}

Mapper033::~Mapper033()
{
}

std::vector<std::string> Mapper033::getInfoStrings()
{
    if (updateInfo)
    {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 033");
        sprintf(line, "PRG BANK @ $8000: %d", prgBank[0]);
        infoString.push_back(std::string(line));
        sprintf(line, "PRG BANK @ $A000: %d", prgBank[1]);
        infoString.push_back(std::string(line));
        sprintf(line, " 2K CHR  @ $0000: %d", chrBank[0]);
        infoString.push_back(std::string(line));
        sprintf(line, " 2K CHR  @ $0800: %d", chrBank[1]);
        infoString.push_back(std::string(line));
        sprintf(line, " 1K CHR  @ $1000: %d", chrBank[2]);
        infoString.push_back(std::string(line));
        sprintf(line, " 1K CHR  @ $1400: %d", chrBank[3]);
        infoString.push_back(std::string(line));
        sprintf(line, " 1K CHR  @ $1800: %d", chrBank[4]);
        infoString.push_back(std::string(line));
        sprintf(line, " 1K CHR  @ $1C00: %d", chrBank[5]);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper033::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xA000, 1);
    prgROM->setBank(0xC000, prgBanks - 2);
    prgROM->setBank(0xE000, prgBanks - 1);

    updateInfo = true;
}

void Mapper033::apply()
{
    // 8K PRG ROM Banks
    prgROM->setBank(0x8000, prgBank[0]);
    prgROM->setBank(0xA000, prgBank[1]);

    //printf("Set prgbank @ $8000 to %d\n", prgBank[0]);
    //printf("Set prgbank @ $A000 to %d\n", prgBank[1]);

    // 2K CHR ROM Banks
    chrROM->setBank(0x0000, (chrBank[0] * 2));
    chrROM->setBank(0x0400, (chrBank[0] * 2) + 1);
    chrROM->setBank(0x0800, (chrBank[1] * 2));
    chrROM->setBank(0x0C00, (chrBank[1] * 2) + 1);

    // 1K CHR ROM Banks
    chrROM->setBank(0x1000, chrBank[2]);
    chrROM->setBank(0x1400, chrBank[3]);
    chrROM->setBank(0x1800, chrBank[4]);
    chrROM->setBank(0x1C00, chrBank[5]);
}

bool Mapper033::getMirrorType(int &data)
{
    data = (mirror == 1 ? Cartridge::Mirror::HORIZONTAL : Cartridge::Mirror::VERTICAL);
    return true;
}

bool Mapper033::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000)
    {
        return true;
    }

    return false;
}

bool Mapper033::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper033::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr < 0xC000)
    {
        switch (addr & 0xA003)
        {
        case 0x8000:
            //printf("%02X written to $8000\n", data);
            mirror = data & 0x40;
            prgBank[0] = data & (prgBanks - 1);
            break;
        case 0x8001:
            prgBank[1] = data & (prgBanks - 1);
            break;
        case 0x8002:
            chrBank[0] = data;
            break;
        case 0x8003:
            chrBank[1] = data;
            break;
        case 0xA000:
            chrBank[2] = data;
            break;
        case 0xA001:
            chrBank[3] = data;
            break;
        case 0xA002:
            chrBank[4] = data;
            break;
        case 0xA003:
            chrBank[5] = data;
            break;
        }
        apply();
        updateInfo = true;
        return true;
    }

    return false;
}

bool Mapper033::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper033::ppuReadData(uint16_t addr, uint8_t &data)
{
    return false;
}

bool Mapper033::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper033::ppuWriteData(uint16_t addr, uint8_t data)
{
    return false;
}
