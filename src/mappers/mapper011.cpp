#include <iostream>
#include "mapper011.h"



Mapper011::Mapper011(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    //vram.resize(0x2000);
}

Mapper011::~Mapper011()
{
}

bool Mapper011::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x8000) {
        mapped_addr = (addr - 0x8000) + (selectedPrgBank * 0x8000);
        return true;
    }
    return false;
}

bool Mapper011::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper011::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        selectedPrgBank = data & 0b00000011;
        selectedChrBank = (data & 0b11110000) >> 4;
        //printf("Mapper011: write %02x to %04x   selectedPrgBank = %d  selectedChrBank = %d\n", data, addr, selectedPrgBank, selectedChrBank);
        return true;
    }

    return false;
}

bool Mapper011::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000) {
        mapped_addr = addr + (selectedChrBank * 0x2000);
        return true;
    }
    return false;
}

bool Mapper011::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr < 0x2000) {
    //    printf("VRAM READ\n");
    //    data = vram[addr];
    //    return true;
    //}
    return false;
}

bool Mapper011::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    // return false? because no RAM?
    if (addr < 0x2000) {
        mapped_addr = addr + (selectedChrBank * 0x2000);
        return true;
    }
    return false;
}

bool Mapper011::ppuWriteData(uint16_t addr, uint8_t data)
{
    //if (addr < 0x2000) {
    //    printf("VRAM WRITE\n");
    //    vram[addr] = data;
    //    return true;
    //}
    return false;
}


