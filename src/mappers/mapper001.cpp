#include <string>
#include "mapper001.h"


Mapper001::Mapper001(uint8_t p, uint8_t c) : Mapper(p, c)
{
    writes = tmpreg = reg[1] = reg[2] = reg[3] = 0;
    reg[0] = 0x0C;
    lastBankOffset = (p - 1) * 0x4000;
    vram.resize(0x2000);
}

Mapper001::~Mapper001()
{
}

bool Mapper001::getMirrorType(int &data)
{
    data = reg[0] & 0x03;
    return true;
}

bool Mapper001::cpuRead(uint16_t addr, uint32_t &mapped_addr)
{
    uint8_t mode = reg[0] & 0b1100;
    if (mode == 0xC) {
        // Highest bank fixed, swapable bank at 0x8000
        if (addr >= 0x8000 && addr < 0xC000) {
            mapped_addr = (addr & 0x7FFF) + ((reg[3] & 0xF) * 0x4000);
            //printf("addr: %04X\tmapped_addr: %05X\n", addr, mapped_addr);
            return true;
        } else if (addr >= 0xC000) {
            mapped_addr = (addr - 0xC000) + lastBankOffset;
            //printf("addr: %04X\tmapped_addr: %05X\n", addr, mapped_addr);
            return true;
        }
    } else if (mode == 0x8) {
        // Lowest bank fixed, swapable bank at 0xC000
        if (addr >= 0x8000 && addr < 0xC000) {
            mapped_addr =  addr - 0x8000;
            return true;
        } else if (addr >= 0xC000) {
            mapped_addr = (addr & 0x3FFF) + ((reg[3] & 0xF) * 0x4000);
            return true;
        }
    } else if (mode == 0x0 || mode == 0x1) {
        // Swapable bank at 0x8000, size 0x8000
        if (addr >= 0x8000) {
            mapped_addr = (addr - 0x8000) + (((reg[3] & 0xF) >> 1) * 0x8000);
            //printf("addr = %04X\tmapped_addr = %05X\n", addr, mapped_addr);
            return true;
        }
    } else {
        printf("[Mapper001] WARNING unimplemented mode %X\n", mode);
        return false;
    }
    return false;
}

bool Mapper001::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x6000 && addr < 0x8000) {
        mapped_addr = addr - 0x6000;
        return true;
    }
    return false;
}

bool Mapper001::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr & 0x8000) {
        if (data & 0x80) {
            // Reset
            writes = 0;
            tmpreg = 0;
            reg[0] |= 0x0C;
            // apply()
        } else {
            tmpreg = ((data & 0x01) << 4) | (tmpreg >> 1);
            if (++writes == 5) {
                //printf("[Mapper001] reg[%d] set to %02X (%02X)\n", (addr >> 13) & 0b11, tmpreg, (tmpreg & 0xF) >> 1);
                reg[(addr >> 13) & 0b11] = tmpreg;
                writes = 0;
                tmpreg = 0;
                // apply()
            }
        }

        return true;
        }

    return false;
}

bool Mapper001::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    uint8_t fourKMode = (reg[0] & 0x10) >> 4;
    if (chrBanks > 0) {
        if (addr < 0x2000) {
            if (fourKMode) {
                if (addr < 0x1000)
                    mapped_addr = (addr & 0x1FFF) + (reg[1] * 0x1000);
                else if (addr < 0x2000)
                    mapped_addr = (addr & 0x0FFF) + (reg[2] * 0x1000);
            } else {
                mapped_addr = (addr & 0x1FFF) + ((reg[1] >> 1) * 0x2000);
            }
            return true;
        }
    }
    return false;
}

bool Mapper001::ppuReadData(uint16_t addr, uint8_t &data)
{
    if (addr < 0x2000) {
        data = vram[addr];
        return true;
    }

    return false;
}

bool Mapper001::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    //printf("Mapper001::ppuWrite %04X\n", addr);
    //if (chrBanks > 0) {
    //    if (addr < 0x2000) {
    //        mapped_addr = addr;
    //        return true;
    //    }
    //} else {
    //    if (addr < 0x2000) {

    //}
    return false;
}

bool Mapper001::ppuWriteData(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000) {
        vram[addr] = data;
        return true;
    }

    return false;
}
