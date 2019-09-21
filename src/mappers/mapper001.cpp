#include "mapper001.h"


Mapper001::Mapper001(uint8_t p, uint8_t c) : Mapper(p, c)
{
}

Mapper001::~Mapper001()
{
}

bool Mapper001::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000) {
        //mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper001::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000) {
        //mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper001::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper001::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr <= 0x1FFF) {
        if (chrBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}

