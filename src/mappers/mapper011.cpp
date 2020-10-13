#include "mapper011.h"
#include <iostream>

Mapper011::Mapper011(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    //vram.resize(0x2000);
}

Mapper011::~Mapper011()
{
}

std::vector<std::string> Mapper011::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 011");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);
        infoString.push_back("PRG Bank offset: 0x8000");
        infoString.push_back("PRG Bank size:   0x8000");
        sprintf(line, "PRG Bank:        %d", selectedPrgBank);
        infoString.push_back(std::string(line));
        infoString.push_back("CHR Bank offset: 0x0000");
        infoString.push_back("CHR Bank size:   0x2000");
        sprintf(line, "CHR BANK:        %d", selectedChrBank);
        infoString.push_back(std::string(line));

        updateInfo = false;
    }

    return infoString;
}

void Mapper011::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xC000, 1);

    chrROM->setBank(0x0000, 0);
    chrROM->setBank(0x1000, 1);

    updateInfo = true;
}

void Mapper011::apply()
{
    prgROM->setBank(0x8000, (selectedPrgBank * 2) + 0);
    prgROM->setBank(0xC000, (selectedPrgBank * 2) + 1);

    chrROM->setBank(0x0000, (selectedChrBank * 2) + 0);
    chrROM->setBank(0x1000, (selectedChrBank * 2) + 1);

    updateInfo = true;
}

bool Mapper011::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
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

        apply();
        //printf("Mapper011: write %02x to %04x   selectedPrgBank = %d  selectedChrBank = %d\n", data, addr, selectedPrgBank, selectedChrBank);
        return true;
    }

    return false;
}

bool Mapper011::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000) {
        return true;
    }
    return false;
}

bool Mapper011::ppuReadData(uint16_t addr, uint8_t &data)
{
    return false;
}

bool Mapper011::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    // return false? because no RAM?
    if (addr < 0x2000) {
        return true;
    }
    return false;
}

bool Mapper011::ppuWriteData(uint16_t addr, uint8_t data)
{
    return false;
}
