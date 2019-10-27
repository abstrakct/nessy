#include <iostream>
#include "mapper013.h"



Mapper013::Mapper013(uint8_t p, uint8_t c) : Mapper(p, c)
{
    vram = std::make_shared<BankedMemory>(4, 0x1000, true);
    chrBank = 1;
    vram->setBank(0x0000, 0);
    vram->setBank(0x1000, chrBank);
}

Mapper013::~Mapper013()
{
}

std::vector<std::string> Mapper013::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();
        
        infoString.push_back("MAPPER 013:");
        sprintf(line, "CHR BANK @ $1000: %d", chrBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper013::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xC000, 1);

    vram->setBank(0x0000, 0);
    vram->setBank(0x1000, 1);

    updateInfo = true;
}


bool Mapper013::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }

    return false;
}

bool Mapper013::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper013::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        chrBank = (data & 0x3);
        vram->setBank(0x1000, chrBank);
        //printf("chr bank set to %d (%d)\n", chrBank, data);
        updateInfo = true;
        return true;
    }

    return false;
}

bool Mapper013::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper013::ppuReadData(uint16_t addr, uint8_t &data)
{
    if (addr < 0x2000) {
        //printf("VRAM READ addr %04X\n", addr);
        data = vram->read(addr);
        return true;
    }
    return false;
}

bool Mapper013::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper013::ppuWriteData(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) {
        //printf("VRAM WRITE %04X %02X\n", addr, data);
        vram->write(addr, data);
        return true;
    }
    return false;
}
