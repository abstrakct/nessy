#include "mapper066.h"
#include <iostream>

Mapper066::Mapper066(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    //vram.resize(0x2000);
}

Mapper066::~Mapper066()
{
}

void Mapper066::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xC000, 1);

    chrROM->setBank(0x0000, 0);
    chrROM->setBank(0x1000, 1);
}

std::vector<std::string> Mapper066::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 066");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(std::string(line));
        sprintf(line, "PRG BANK: %d", selectedPrgBank);
        infoString.push_back(std::string(line));
        sprintf(line, "CHR BANK: %d", selectedChrBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }
    return infoString;
}

void Mapper066::apply()
{
    prgROM->setBank(0x8000, (selectedPrgBank * 2) + 0);
    prgROM->setBank(0xC000, (selectedPrgBank * 2) + 1);

    chrROM->setBank(0x0000, (selectedChrBank * 2) + 0);
    chrROM->setBank(0x1000, (selectedChrBank * 2) + 1);

    updateInfo = true;
}

bool Mapper066::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }
    return false;
}

bool Mapper066::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper066::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000) {
        selectedChrBank = data & 0b00000011;
        selectedPrgBank = (data & 0b00110000) >> 4;

        apply();
        //printf("Mapper066: write %02x to %04x   selectedPrgBank = %d  selectedChrBank = %d\n", data, addr, selectedPrgBank, selectedChrBank);
        return true;
    }

    return false;
}

bool Mapper066::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;
    return false;
}

bool Mapper066::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr < 0x2000) {
    //    printf("VRAM READ\n");
    //    data = vram[addr];
    //    return true;
    //}
    return false;
}

bool Mapper066::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    //if (addr < 0x2000) {
    //    mapped_addr = addr + (selectedChrBank * 0x2000);
    //    return true;
    //}
    return false;
}

bool Mapper066::ppuWriteData(uint16_t addr, uint8_t data)
{
    //if (addr < 0x2000) {
    //    printf("VRAM WRITE\n");
    //    vram[addr] = data;
    //    return true;
    //}
    return false;
}
