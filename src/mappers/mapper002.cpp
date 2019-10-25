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
//
// Carts with this mapper have no CHR ROM, but 8K CHR RAM / VRAM

Mapper002::Mapper002(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // Set offset to first byte of the last bank.
    lastBank = p - 1;

    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);
}

Mapper002::~Mapper002()
{
}

std::vector<std::string> Mapper002::getInfoStrings()
{
    return infoString;
}

void Mapper002::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xC000, lastBank);
}


bool Mapper002::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }

    return false;
}

bool Mapper002::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper002::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        selectedBank = data & 0b00000111;  // last 3 bits select bank. Could also be written as & 0x07
        prgROM->setBank(0x8000, selectedBank);
        
        //printf("Mapper002: write %02x to %04x   selectedBank = %d\n", data, addr, selectedBank);
        //
        // TODO: if 'UOROM' variety, 4 bits are used to select bank.
        return true;
    }

    return false;
}

bool Mapper002::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper002::ppuReadData(uint16_t addr, uint8_t &data)
{
    if (addr < 0x2000) {
        //printf("VRAM READ\n");
        data = vram[addr];
        return true;
    }
    return false;
}

bool Mapper002::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper002::ppuWriteData(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) {
        //printf("VRAM WRITE %04X %02X\n", addr, data);
        vram[addr] = data;
        return true;
    }
    return false;
}
