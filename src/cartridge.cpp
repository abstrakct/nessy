
#include "cartridge.h"

Cartridge::Cartridge()
{
}

Cartridge::~Cartridge()
{
}

uint8_t Cartridge::cpuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    return data;
}

void Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
}

uint8_t Cartridge::ppuRead(uint16_t addr, bool readOnly)
{
    uint8_t data = 0x00;

    return data;
}

void Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
}

