#include "mapper013.h"
#include <iostream>

Mapper013::Mapper013(uint8_t p, uint8_t c) : Mapper(p, c)
{
    chrBank = 7;
    vram = std::make_shared<BankedMemory>("VRAM", 8, 0x0400, true);
    vram->setMappable(8);
    vram->setBank(0x0000, 0, 0);
    vram->setBank(0x0400, 1, 1);
    vram->setBank(0x0800, 2, 2);
    vram->setBank(0x0C00, 3, 3);
    vram->setBank(0x1000, chrBank - 3, 4);
    vram->setBank(0x1400, chrBank - 2, 5);
    vram->setBank(0x1800, chrBank - 1, 6);
    vram->setBank(0x1C00, chrBank, 7);
}

Mapper013::~Mapper013()
{
}

std::vector<std::string> Mapper013::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 013");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);
        infoString.push_back("CHR Bank offset: 0x1000");
        infoString.push_back("CHR Bank size:   0x1000");
        sprintf(line, "CHR BANK:        %d", chrBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper013::reset()
{
    prgROM->setMappable(2);
    prgROM->setBank(0x8000, 0, 0);
    prgROM->setBank(0xC000, 1, 1);

    vram->setBank(0x0000, 0, 0);
    vram->setBank(0x0400, 1, 1);
    vram->setBank(0x0800, 2, 2);
    vram->setBank(0x0C00, 3, 3);
    vram->setBank(0x1000, chrBank - 3, 4);
    vram->setBank(0x1400, chrBank - 2, 5);
    vram->setBank(0x1800, chrBank - 1, 6);
    vram->setBank(0x1C00, chrBank, 7);

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
