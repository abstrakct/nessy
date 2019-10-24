#include "mapper000.h"


Mapper000::Mapper000(uint8_t p, uint8_t c) : Mapper(p, c)
{
}

Mapper000::~Mapper000()
{
}

void Mapper000::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xC000, 1);

    chrROM->setBank(0x0000, 0);
    chrROM->setBank(0x1000, 1);
}

bool Mapper000::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }
    return false;
}

bool Mapper000::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000) {
        printf("WRITING TO ROM?!?!?!\n");
        mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper000::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000) {
        return true;
    }
    return false;
}

bool Mapper000::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000) {
        if (chrBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}
