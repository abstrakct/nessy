#include "mapper_000.h"


Mapper_000::Mapper_000(uint8_t p, uint8_t c) : Mapper(p, c)
{
}

Mapper_000::~Mapper_000()
{
}

bool Mapper_000::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper_000::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool Mapper_000::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper_000::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (chrBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}
