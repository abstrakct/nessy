#include "mapper001.h"
#include <string>

Mapper001::Mapper001(uint8_t p, uint8_t c) : Mapper(p, c)
{
    writes = tmpreg = reg[1] = reg[2] = reg[3] = 0;
    reg[0] = 0x0C;
    lastBank = p - 1;
    vram.resize(0x2000);
}

Mapper001::~Mapper001()
{
}

std::vector<std::string> Mapper001::getInfoStrings()
{
    if (updateInfo) {
        char line[50];
        uint8_t prgMode = ((reg[0] & 0b1100) >> 2);
        uint8_t chrMode = reg[0] & 0x10;
        infoString.clear();

        infoString.push_back("MAPPER 001");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);

        if (prgMode <= 1) {
            infoString.push_back("PRGROM: Switch 32K @ $8000");
            sprintf(line, "Selected bank: %d", reg[3] >> 1);
            infoString.push_back(std::string(line));
        } else if (prgMode == 2) {
            infoString.push_back("PRGROM: Switch 16K @ $C000");
            sprintf(line, "Selected bank: %d", reg[3] & 0xF);
            infoString.push_back(std::string(line));
        } else if (prgMode == 3) {
            infoString.push_back("PRGROM: Switch 16K @ $8000");
            sprintf(line, "Selected bank: %d", reg[3] & 0xF);
            infoString.push_back(std::string(line));
        }

        if (chrBanks > 0) {
            if (chrMode == 0) {
                infoString.push_back("CHRROM: Switch 1 x 8K Bank");
                sprintf(line, "Selected bank: %d", reg[1] >> 1);
                infoString.push_back(std::string(line));
            } else if (chrMode == 1) {
                infoString.push_back("CHRROM: Switch 2 x 4K Banks");
                sprintf(line, "Bank @ $0000: %d", reg[1] & 0x1F);
                infoString.push_back(std::string(line));
                sprintf(line, "Bank @ $1000: %d", reg[2] & 0x1F);
                infoString.push_back(std::string(line));
            }
        } else {
            infoString.push_back("No CHR ROM on cart");
        }

        updateInfo = false;
    }

    return infoString;
}

void Mapper001::reset()
{
    prgROM->setMappable(2);
    chrROM->setMappable(2);

    if (prgROM) {
        prgROM->setBank(0x8000, 0, 0);
        prgROM->setBank(0xC000, lastBank, 1);
    }

    if (chrROM) {
        chrROM->setBank(0x0000, 0, 0);
        chrROM->setBank(0x0000, 1, 1);
    }
}

void Mapper001::apply()
{
    // PRG ROM
    uint8_t mode = (reg[0] & 0b1100) >> 2;
    if (mode == 0x3) {
        prgROM->setBank(0x8000, reg[3] & 0xF, 0);
        prgROM->setBank(0xC000, lastBank, 1);
    } else if (mode == 0x2) {
        prgROM->setBank(0x8000, 0, 0);
        prgROM->setBank(0xC000, reg[3] & 0xF, 1);
    } else if (mode < 0x2) {
        prgROM->setBank(0x8000, (reg[3] >> 1) + 0, 0);
        prgROM->setBank(0xC000, (reg[3] >> 1) + 1, 1);
    }

    // CHR ROM
    uint8_t fourKMode = (reg[0] & 0x10);
    if (fourKMode) {
        // Switch 2 separate 4 KB banks
        chrROM->setBank(0x0000, reg[1] & 0x1F, 0);
        chrROM->setBank(0x1000, reg[2] & 0x1F, 0);
    } else {
        // Switch 1 8 KB bank
        chrROM->setBank(0x0000, ((reg[1] >> 1) << 1) + 0, 0);
        chrROM->setBank(0x1000, ((reg[1] >> 1) << 1) + 1, 1);
    }

    updateInfo = true;
}

bool Mapper001::getMirrorType(int &data)
{
    data = reg[0] & 0b11;

    return true;
}

bool Mapper001::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    // PRG RAM?
    if (addr >= 0x6000 && addr < 0x8000) {
        mapped_addr = addr - 0x6000;
        prgram = true;
        return true;
    } else if (addr >= 0x8000) {
        prgram = false;
        return true;
    }

    prgram = false;
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
            apply();
        } else {
            tmpreg = ((data & 1) << 4) | (tmpreg >> 1);
            if (++writes == 5) {
                //printf("[Mapper001] reg[%d] set to %02X (%02X)\n", (addr >> 13) & 0b11, tmpreg, (tmpreg & 0xF) >> 1);
                reg[(addr >> 13) & 0b11] = tmpreg;
                writes = 0;
                tmpreg = 0;
                apply();
            }
        }

        return true;
    }

    return false;
}

bool Mapper001::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    // uint8_t fourKMode = (reg[0] & 0x10) >> 4;
    if (chrBanks > 0) {
        if (addr < 0x2000) {
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
