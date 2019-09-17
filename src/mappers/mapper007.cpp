#include <iostream>
#include "mapper007.h"


// CPU $8000 - $FFFF: 32 KB switchable PRG ROM bank
//
// PRG ROMS:
// Bank 1: $0000 - $8000
// Bank 2: $4000 - $C000
// ...
// Bank 8: $38000 - $40000
//
// Carts with this mapper have no CHR ROM, but 8K CHR RAM / VRAM

Mapper007::Mapper007(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);
}

Mapper007::~Mapper007()
{
}

bool Mapper007::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000) {
        mapped_addr = (addr - 0x8000) + (selectedBank * 0x8000);
        return true;
    }
    return false;
}

bool Mapper007::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper007::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        selectedBank = data & 0b00000111;  // last 3 bits select bank. Could also be written as & 0x07
        //TODO: bit 5 = select 1 kb VRAM page for all 4 nametables
        //printf("Mapper007: write %02x to %04x   selectedBank = %d\n", data, addr, selectedBank);
        return true;
    }

    return false;
}

bool Mapper007::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper007::ppuReadData(uint16_t addr, uint8_t &data)
{
    if (addr < 0x2000) {
        printf("VRAM READ\n");
        data = vram[addr];
        return true;
    }
    return false;
}

bool Mapper007::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper007::ppuWriteData(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) {
        printf("VRAM WRITE\n");
        vram[addr] = data;
        return true;
    }
    return false;
}

