#include "mapper007.h"
#include <iostream>

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

std::vector<std::string> Mapper007::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 007");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);
        infoString.push_back("PRG Bank offset: 0x8000");
        infoString.push_back("PRG Bank size:   0x8000");
        sprintf(line, "PRG Bank:        %d", prgBank);
        infoString.push_back(std::string(line));
        sprintf(line, "VRAM Bank:       %d", vramBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }

    return infoString;
}

bool Mapper007::getMirrorType(int &data)
{
    data = vramBank;
    return true;
}

void Mapper007::reset()
{
    prgROM->setMappable(2);
    prgROM->setBank(0x8000, 0, 0);
    prgROM->setBank(0xC000, 1, 1);

    updateInfo = true;
}

void Mapper007::apply()
{
    prgROM->setBank(0x8000, prgBank * 2, 0);
    prgROM->setBank(0xC000, (prgBank * 2) + 1, 1);

    updateInfo = true;
}

bool Mapper007::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        //mapped_addr = (addr - 0x8000) + (prgBank * 0x8000);
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
        prgBank = data & 0b00000111; // last 3 bits select bank. Could also be written as & 0x07
        vramBank = (data & 0b00010000) >> 4;

        apply();

        //printf("Mapper007: write %02x to %04x   selectedBank = %d   vramBank = %d\n", data, addr, selectedBank, vramBank);
        return true;
    }

    return false;
}

// Taken from ANESE
//inline uint16_t nt_mirror(uint16_t addr) const
//{
//    const uint16_t fix_4s = 0x2000 * (
//}

bool Mapper007::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper007::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr > 0x0000 && addr < 0x3EFF) {
    //    //printf("VRAM READ addr %04X\n", addr);
    //    if (addr >= 0x2000) {
    //        if (vramBank) {
    //            if (addr >= 0x2400) {
    //                addr &= 0x23FF;
    //                addr += 0x400;
    //            }
    //        } else {
    //            addr &= 0x23FF;
    //        }
    //    }
    //    addr &= 0x1FFF;
    //    data = vram[addr];
    //    return true;
    //}

    if (addr < 0x2000) {
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
    //printf("VRAM WRITE addr %04X\n", addr);
    //if (addr > 0x0000 && addr < 0x3EFF) {
    //    if (addr >= 0x2000) {
    //        if (vramBank) {
    //            if (addr >= 0x2400) {
    //                addr &= 0x23FF;
    //                addr += 0x400;
    //            }
    //        } else {
    //            addr &= 0x23FF;
    //        }
    //    }

    //    //printf("VRAM WRITE addr %04X   data %02X\n", addr, data);
    //    addr &= 0x1FFF;
    //    vram[addr] = data;
    //    return true;
    //}

    if (addr < 0x2000) {
        vram[addr] = data;
    }

    return false;
}
