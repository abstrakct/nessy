#include "mapper004.h"
#include <iostream>

Mapper004::Mapper004(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);

    lastBank = p - 1;
    reg[0] = 0;
}

Mapper004::~Mapper004()
{
}

std::vector<std::string> Mapper004::getInfoStrings()
{
    if (updateInfo) {
        char line[50];

        infoString.clear();

        infoString.push_back("MAPPER 004");
        sprintf(line, "Mapper emulation status: %s", implementationStatusDescription(this->implementationStatus()));
        infoString.push_back(line);
        sprintf(line, "Command: %d", command);
        infoString.push_back(line);
        sprintf(line, "chraddrsel: %d", chrAddrSel);
        infoString.push_back(line);
        sprintf(line, "prgaddrsel: %d", prgAddrSel);
        infoString.push_back(line);
        // sprintf(line, "CHR BANK: %d", chrBank);

        updateInfo = false;
    }
    return infoString;
}

void Mapper004::reset()
{
    prgROM->setBank(0x8000, 0);
    prgROM->setBank(0xA000, 1);
    prgROM->setBank(0xC000, lastBank - 1);
    prgROM->setBank(0xE000, lastBank);

    //chrROM->setBank(0x0000, 0);
    //chrROM->setBank(0x1000, 1);

    updateInfo = true;
}

void Mapper004::apply()
{
    //chrROM->setBank(0x0000, (chrBank * 2) + 0);
    //chrROM->setBank(0x1000, (chrBank * 2) + 1);

    updateInfo = true;
}

bool Mapper004::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    prgram = false;
    if (addr >= 0x8000) {
        return true;
    }
    return false;
}

bool Mapper004::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper004::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr < 0xA000) {
        if (addr == 0x8000) {
            // CPxxxNNN
            // C = CHR address select
            // P = PRG address select
            // N = command number
            command = data & 0x7;
            prgAddrSel = data & 0x40;
            chrAddrSel = data & 0x80;
        } else if (addr == 0x8001) {
            // data = page number for command
            // activates command selected
        } else if (addr == 0xA000) {
            // mirroring select
        } else if (addr == 0xA001) {
            // saveram toggle
        } else if (addr == 0xC000) {
            irqCounter = data;
        } else if (addr == 0xC001) {
            // irq latch register
        } else if (addr == 0xE000) {
            // irq ctrl register 0
        } else if (addr == 0xE001) {
            // irq ctrl register 1
        }
        return true;
    }

    return false;
}

bool Mapper004::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper004::ppuReadData(uint16_t addr, uint8_t &data)
{
    //if (addr < 0x2000) {
    //    data = vram[addr];
    //    return true;
    //}
    return false;
}

bool Mapper004::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}

bool Mapper004::ppuWriteData(uint16_t addr, uint8_t data)
{
    //if (addr < 0x2000) {
    //    vram[addr] = data;
    //    return true;
    //}
    return false;
}

bool Mapper004::irqState()
{
    return false;
}

void Mapper004::irqClear()
{
}

void Mapper004::scanline()
{
}
