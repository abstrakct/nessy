#include <iostream>
#include "mapper002.h"


// CPU $8000 - $BFFF: 16 KB switchable PRG ROM bank
// CPU $C000 - $FFFF: 16 KB PRG ROM bank, fixed to the last bank
//
// PRG ROMS:
// Bank 1: $0000 - $4000
// Bank 2: $4000 - $8000
// ...
// Bank 8: $1C000 - $20000

Mapper002::Mapper002(uint8_t p, uint8_t c) : Mapper(p, c)
{
    lastBankOffset = (p - 1) * 0x4000;
}

Mapper002::~Mapper002()
{
}

bool Mapper002::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000 && addr <= 0xBFFF) {
        mapped_addr = (addr - 0x8000) + (selectedBank * 0x4000);
        //mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    } else if(addr >= 0xC000) {
        mapped_addr = (addr - 0xC000) + lastBankOffset;
        //printf("cpuRead: addr = %04X  mapped_addr = %06X\n", addr, mapped_addr);
        return true;
    }
    return false;
}

bool Mapper002::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    //if (addr >= 0x8000) {
    //    mapped_addr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
    //    return true;
    //}
    return false;
}

bool Mapper002::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        selectedBank = data & 0b00000111;  // last 3 bits select bank. Could also be written as & 0x07
        //printf("Mapper002: write %02x to %04x   selectedBank = %d\n", data, addr, selectedBank);
        return true;
    }

    return false;
}

bool Mapper002::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        mapped_addr = addr;
        return true;
    }
    return false;
}

bool Mapper002::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        if (chrBanks == 0) {
            mapped_addr = addr;
            return true;
        }
    }
    return false;
}

