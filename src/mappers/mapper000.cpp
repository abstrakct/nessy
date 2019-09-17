#include "mapper000.h"


Mapper000::Mapper000(uint8_t p, uint8_t c) : Mapper(p, c)
{
}

Mapper000::~Mapper000()
{
}

bool Mapper000::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper000::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper000::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper000::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (chrBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}
