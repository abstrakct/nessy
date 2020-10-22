#include "mapper004.h"
#include "../cartridge.h"
#include <iostream>

Mapper004::Mapper004(uint8_t p, uint8_t c) : Mapper(p, c)
{
    // 8K VRAM (CHR RAM)
    vram.resize(0x2000);

    lastBank = p - 1;
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
        sprintf(line, "PRG Bank Mode: %s", prgBankMode ? "0xC000 swappable" : "0x8000 swappable");
        infoString.push_back(line);
        sprintf(line, "CHR Bank Inv:  %s", chrInversion ? "true" : "false");
        infoString.push_back(line);
        sprintf(line, "Registers: %02X %02X %02X %02X %02X %02X %02X %02X", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7]);
        infoString.push_back(line);
        sprintf(line, "IRQ Counter: %d", irqCounter);
        infoString.push_back(line);
        sprintf(line, "IRQ Enable: %s", irqEnable ? "true" : "false");
        infoString.push_back(line);
        sprintf(line, "IRQ Active: %s", irqActive ? "true" : "false");
        infoString.push_back(line);
        sprintf(line, "Mirror mode: %s", mirror == Cartridge::Mirror::HORIZONTAL ? "Horizontal" : "Vertical");
        infoString.push_back(line);

        updateInfo = false;
    }
    return infoString;
}

void Mapper004::reset()
{
    targetRegister = 0;
    prgBankMode = false;
    chrInversion = false;
    mirror = Cartridge::Mirror::HORIZONTAL;
    irqActive = false;
    irqEnable = false;
    irqUpdate = false;
    irqCounter = 0;
    irqReload = 0;

    prgROM->setMappable(4);
    chrROM->setMappable(8);

    prgROM->setBank(0x8000, 0, 0);
    prgROM->setBank(0xA000, 1, 1);
    prgROM->setBank(0xC000, lastBank - 1, 2);
    prgROM->setBank(0xE000, lastBank, 3);

    for (int i = 0; i < 8; i++) {
        chrROM->setBank(i * 0x400, 0);
    }

    updateInfo = true;
}

void Mapper004::apply()
{
    if (prgBankMode) {
        prgROM->setBank(0x8000, lastBank - 1, 0);
        prgROM->setBank(0xA000, reg[7] & 0x3F, 1);
        prgROM->setBank(0xC000, reg[6] & 0x3F, 2);
    } else {
        prgROM->setBank(0x8000, reg[6] & 0x3F, 0);
        prgROM->setBank(0xA000, reg[7] & 0x3F, 1);
        prgROM->setBank(0xC000, lastBank - 1, 2);
    }

    if (chrInversion) {
        chrROM->setBank(0x0000, reg[2], 0);
        chrROM->setBank(0x0400, reg[3], 1);
        chrROM->setBank(0x0800, reg[4], 2);
        chrROM->setBank(0x0C00, reg[5], 3);
        chrROM->setBank(0x1000, (reg[0] & 0xFE), 4);
        chrROM->setBank(0x1400, (reg[0] & 0xFE) + 1, 5);
        chrROM->setBank(0x1800, (reg[1] & 0xFE), 6);
        chrROM->setBank(0x1C00, (reg[1] & 0xFE) + 1, 7);
    } else {
        chrROM->setBank(0x0000, (reg[0] & 0xFE), 0);
        chrROM->setBank(0x0400, (reg[0] & 0xFE) + 1, 1);
        chrROM->setBank(0x0800, (reg[1] & 0xFE), 2);
        chrROM->setBank(0x0C00, (reg[1] & 0xFE) + 1, 3);
        chrROM->setBank(0x1000, reg[2], 4);
        chrROM->setBank(0x1400, reg[3], 5);
        chrROM->setBank(0x1800, reg[4], 6);
        chrROM->setBank(0x1C00, reg[5], 7);
    }

    updateInfo = true;
}

bool Mapper004::cpuRead(uint16_t addr, uint32_t &mapped_addr, bool &prgram)
{
    if (addr >= 0x6000 && addr < 0x8000) {
        prgram = true;
        mapped_addr = addr & 0x1FFF;
        return true;
    }
    if (addr >= 0x8000) {
        return true;
    }
    return false;
}

bool Mapper004::cpuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x6000 && addr < 0x8000) {
        mapped_addr = addr & 0x1FFF;
        return true;
    }
    return false;
}

bool Mapper004::cpuWriteData(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr < 0xA000) {
        // CPxxxNNN
        // C = CHR address select
        // P = PRG address select
        // N = command number
        if (!(addr & 0x0001)) {
            // even
            targetRegister = data & 0x07;
            prgBankMode = data & 0x40;
            chrInversion = data & 0x80;
        } else {
            // odd
            // printf("MAPPER 004: Writing %02X to register %d\n", data, targetRegister);
            reg[targetRegister] = data;
            apply();
        }
        return true;
    }

    if (addr >= 0xA000 && addr < 0xC000) {
        // mirroring select
        if (!(addr & 0x0001)) {
            if (data & 0x01) {
                mirror = Cartridge::Mirror::HORIZONTAL;
            } else {
                mirror = Cartridge::Mirror::VERTICAL;
            }
        } else {
            // saveram toggle
            // TODO: PRG RAM protect ?!
        }
        return true;
    }

    if (addr >= 0xC000 && addr < 0xE000) {
        if (!(addr & 0x0001)) {
            irqReload = data;
        } else {
            irqCounter = 0;
        }
        return true;
    }

    if (addr >= 0xE000 && addr <= 0xFFFF) {
        if (!(addr & 0x0001)) {
            irqEnable = false;
            irqActive = false;
        } else {
            irqEnable = true;
        }
        return true;
    }

    return false;
}

bool Mapper004::ppuRead(uint16_t addr, uint32_t &mapped_addr)
{
    // printf("mapper004 ppuread addr %04x\n", addr);
    if (addr < 0x2000)
        return true;

    return false;
}

bool Mapper004::ppuReadData(uint16_t addr, uint8_t &data)
{
    // printf("ppureaddata addr %04x\n", addr);
    // if (addr < 0x2000) {
    //     data = vram[addr];
    //     return true;
    // }
    return false;
}

bool Mapper004::ppuWrite(uint16_t addr, uint32_t &mapped_addr)
{
    // printf("ppuwrite\n");
    return false;
}

bool Mapper004::ppuWriteData(uint16_t addr, uint8_t data)
{
    // printf("ppuwritedata addr %04x\n", addr);
    // if (addr < 0x2000) {
    //     vram[addr] = data;
    //     return true;
    // }
    return false;
}

bool Mapper004::irqState()
{
    return irqActive;
}

void Mapper004::irqClear()
{
    irqActive = false;
}

void Mapper004::scanline()
{
    if (irqCounter == 0) {
        irqCounter = irqReload;
    } else {
        irqCounter--;
    }

    if (irqCounter == 0 && irqEnable) {
        irqActive = true;
    }
}
